#include "wep-elems.h"
#include <string>
#include <openssl/rc4.h>
#include <zlib.h> // For CRC-32 computation
#include <cstdlib>
#include <ctime>

namespace ns3
{

WEPElems::WEPElems()
{
    // Default constructor with no challenge text
    m_wepKeyIndex = 0;
    m_wepICV = 0;
    m_encryptedChallenge.clear();
}

WEPElems::WEPElems(std::string challengeText, std::string wepKey)
{
    // Initialize WEP Key Index, IV, and ICV
    std::srand(std::time(nullptr)); // Seed the random number generator
    m_wepIV[0] = std::rand() % 256;
    m_wepIV[1] = std::rand() % 256;
    m_wepIV[2] = std::rand() % 256;
    m_wepKeyIndex = 0;  // Default key index
    m_wepICV = 0;       // Default ICV (to be calculated)
    m_encryptedChallenge.clear();

    EncryptChallengeText(challengeText, wepKey);
}

void WEPElems::EncryptChallengeText(std::string challengeText, std::string wepKey)
{
    size_t key_length = wepKey.size();

    size_t text_length = challengeText.size(); // Exclude null terminator for CRC computation

    // Compute the CRC-32 checksum (ICV)
    m_wepICV = crc32(0, reinterpret_cast<const unsigned char*>(challengeText.data()), text_length);

    // Append the ICV to the plaintext
    size_t total_length = text_length + sizeof(m_wepICV); // Total length = plaintext + ICV
    unsigned char plaintext_with_icv[total_length];
   
    for (size_t i = 0; i < text_length; ++i) {
        plaintext_with_icv[i] = challengeText[i];
    }

    for (size_t i = 0; i < sizeof(m_wepICV); ++i) {
        plaintext_with_icv[text_length + i] = (m_wepICV >> (8 * (sizeof(m_wepICV) - i - 1))) & 0xFF;
    }

    // Output buffers for encrypted and decrypted data
    unsigned char encrypted_text[total_length];

    // RC4 Key Initialization
    RC4_KEY rc4_key;
    RC4_set_key(&rc4_key, key_length, reinterpret_cast<const unsigned char*>(wepKey.data()));

    // Encrypt the plaintext + ICV
    RC4(&rc4_key, total_length, plaintext_with_icv, encrypted_text);

    std::vector<uint8_t> encryptedVector(encrypted_text, encrypted_text + total_length);

    m_encryptedChallenge = encryptedVector;

}

bool WEPElems::AuthenticateClient(std::string challengeText, std::string wepKey)
{
    size_t key_length = wepKey.size();

    size_t total_length = m_encryptedChallenge.size();

    size_t text_length = challengeText.size();

    unsigned char decrypted_text[total_length];

    RC4_KEY rc4_key;
    RC4_set_key(&rc4_key, key_length, reinterpret_cast<const unsigned char*>(wepKey.data()));

    const unsigned char* encryptedData = reinterpret_cast<const unsigned char*>(m_encryptedChallenge.data());
    RC4(&rc4_key, total_length, encryptedData, decrypted_text);

    // Separate decrypted plaintext and ICV
    std::string decrypted_plaintext(reinterpret_cast<char*>(decrypted_text), text_length);

    // Verify the ICV
    uint32_t computed_icv = crc32(0, reinterpret_cast<const unsigned char*>(decrypted_plaintext.data()), text_length);
    // std::cout << "Decrypted Text: " << decrypted_plaintext << std::endl;
    // std::cout << "Received ICV: " << std::hex << received_icv << std::endl;
    // std::cout << "Computed ICV: " << std::hex << computed_icv << std::dec << std::endl;

    return (m_wepICV == computed_icv && challengeText == decrypted_plaintext);
}

std::vector<uint8_t> WEPElems::PeekEncryptedText() const
{
    return m_encryptedChallenge;
}

uint8_t WEPElems::PeekWEPIV() const
{
    return m_wepIV[3];
}

u_int8_t WEPElems::PeekWEPKeyIndex() const
{
    return m_wepKeyIndex;
}

u_int32_t WEPElems::PeekWEPICV() const
{
    return m_wepICV;
}


WifiInformationElementId WEPElems::ElementId() const
{
    return IE_CHALLENGE_TEXT;
}

uint16_t
WEPElems::GetInformationFieldSize() const
{
    return 3 + 1 + sizeof(m_wepICV) + m_encryptedChallenge.size();  // IV (3 bytes) + Key Index (1 byte) + ICV (4 bytes) + Encrypted Challenge
}

void
WEPElems::SerializeInformationField(Buffer::Iterator start) const
{
    // Serialize WEP IV (3 bytes)
    for (size_t i = 0; i < 3; ++i)
    {
        start.WriteU8(m_wepIV[i]);
    }

    // Serialize WEP Key Index (1 byte)
    start.WriteU8(m_wepKeyIndex);

    // Serialize WEP ICV (4 bytes)
    start.WriteU32(m_wepICV);

    // Serialize Encrypted Challenge Text
    for (size_t i = 0; i < m_encryptedChallenge.size(); ++i)
    {
        start.WriteU8(m_encryptedChallenge[i]);
    }
}

uint16_t
WEPElems::DeserializeInformationField(Buffer::Iterator start, uint16_t length)
{
    uint16_t offset = 0;

    // Deserialize WEP IV (3 bytes)
    for (size_t i = 0; i < 3; ++i)
    {
        m_wepIV[i] = start.ReadU8();
    }
    offset += 3;

    // Deserialize WEP Key Index (1 byte)
    m_wepKeyIndex = start.ReadU8();
    offset += 1;

    // Deserialize WEP ICV (4 bytes)
    m_wepICV = start.ReadU32();
    offset += 4;

    // Deserialize Encrypted Challenge Text
    m_encryptedChallenge.clear();
    while (offset < length)
    {
        m_encryptedChallenge.push_back(start.ReadU8());
        offset++;
    }

    return offset;
}


void 
WEPElems::Print(std::ostream& os) const
{
    os << "WEP IV: ";
    for (size_t i = 0; i < 3; ++i)
    {
        os << std::hex << static_cast<int>(m_wepIV[i]) << " ";
    }
    os << "\nWEP Key Index: " << std::hex << static_cast<int>(m_wepKeyIndex) << "\n";
    os << "Encrypted Challenge: ";
    for (auto byte : m_encryptedChallenge)
    {
        os << std::hex << static_cast<int>(byte) << " ";
    }
    os << "\nWEP ICV: " << std::hex << m_wepICV << std::dec;
}
// ATTRIBUTE_HELPER_CPP(WEPElems);

// std::istream& operator>>(std::istream& is, WEPElems& wep_elems)
// {
//     std::string input;
//     is >> input; // Read input as a string
//     return is;
// }



} // namespace ns3