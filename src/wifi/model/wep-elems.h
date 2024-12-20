#ifndef WEPELEMS_H
#define WEPELEMS_H

#include "wifi-information-element.h"

namespace ns3
{

/**
 * \ingroup wifi
 *
 * The IEEE 802.11 SSID Information Element
 *
 */
class WEPElems : public WifiInformationElement
{
  public:
    
    WEPElems();
  
    WEPElems(std::string challengeText, std::string wepKey);

    // Implementations of pure virtual methods of WifiInformationElement
    WifiInformationElementId ElementId() const override;
    void Print(std::ostream& os) const override;

    void EncryptChallengeText(std::string challengeText, std::string wepKey);

    bool AuthenticateClient(std::string challengeText, std::string wepKey);

    std::vector<uint8_t> PeekEncryptedText() const;

    u_int8_t PeekWEPIV() const;

    u_int8_t PeekWEPKeyIndex() const;

    u_int32_t PeekWEPICV() const;

  private:
    uint16_t GetInformationFieldSize() const override;
    void SerializeInformationField(Buffer::Iterator start) const override;
    uint16_t DeserializeInformationField(Buffer::Iterator start, uint16_t length) override;

    uint8_t m_wepIV[3]{0};                       //!< WEP Initialization Vector
    uint8_t m_wepKeyIndex{0};                    //!< WEP Key Index
    uint32_t m_wepICV{0};
    std::vector<uint8_t> m_encryptedChallenge;
};


// std::istream& operator>>(std::istream& is, WEPElems& wepElems);

// ATTRIBUTE_HELPER_HEADER(WEPElems);

} // namespace ns3

#endif /* WEPELEMS_H */
