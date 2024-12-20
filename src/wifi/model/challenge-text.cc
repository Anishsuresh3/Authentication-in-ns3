#include "challenge-text.h"

namespace ns3
{

ChallengeText::ChallengeText()
{
    m_length = 0;
}

ChallengeText::ChallengeText(std::string s)
{
    // Ensure the input string does not exceed the maximum allowed length
    if (s.size() > sizeof(m_challengetext))
    {
        NS_FATAL_ERROR("ChallengeText string is too long. Maximum allowed is " 
                       << sizeof(m_challengetext) << " bytes.");
    }

    m_length = static_cast<uint8_t>(s.size());
    // Set the length of the challenge text
    m_length = static_cast<uint8_t>(s.size());

    // Resize the vector to match the length
    m_challengetext.resize(m_length, 0); // Initialize to max length, fill with 0s

    // Copy the input string into the vector
    for (uint8_t i = 0; i < m_length; ++i)
    {
        m_challengetext[i] = static_cast<uint8_t>(s[i]);
    }

}


void 
ChallengeText::Print(std::ostream& os) const 
{
    os << "Challenge Text: ";
    for (auto byte : m_challengetext)
    {
        os << std::hex << static_cast<int>(byte) << " ";
    }
    os << std::dec; // Reset back to decimal
}


bool ChallengeText::IsEqual(const ChallengeText& o) const
{
    // Compare the challenge text values
    return m_challengetext == o.m_challengetext;
}


std::string ChallengeText::PeekString() const
{
    return std::string(m_challengetext.begin(), m_challengetext.end());
}

WifiInformationElementId ChallengeText::ElementId() const
{
    return IE_CHALLENGE_TEXT;
}


uint16_t
ChallengeText::GetInformationFieldSize() const
{
    return m_length;
}

void 
ChallengeText::SerializeInformationField(Buffer::Iterator start) const 
{
    for (size_t i = 0; i < m_challengetext.size(); ++i)
    {
        start.WriteU8(m_challengetext[i]);
    }
}

uint16_t 
ChallengeText::DeserializeInformationField(Buffer::Iterator start, uint16_t length)
{
    m_challengetext.clear();
    for (uint16_t i = 0; i < length; ++i)
    {
        m_challengetext.push_back(start.ReadU8());
    }
    return length;
}

void ChallengeText::SetChallengeText(const std::string& text)
{
    m_challengetext.clear();
    for (char c : text)
    {
        m_challengetext.push_back(static_cast<uint8_t>(c));
    }
    m_length = static_cast<uint8_t>(text.size());
}



ATTRIBUTE_HELPER_CPP(ChallengeText);

std::istream& operator>>(std::istream& is, ChallengeText& challenge_text)
{
    std::string input;
    is >> input; // Read input as a string

    challenge_text.SetChallengeText(input);

    return is;
}



} // namespace ns3