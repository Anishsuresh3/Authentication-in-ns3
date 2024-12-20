#ifndef CHALLENGETEXT_H
#define CHALLENGETEXT_H

#include "wifi-information-element.h"

namespace ns3
{

/**
 * \ingroup wifi
 *
 * The IEEE 802.11 SSID Information Element
 *
 * \see challenge_text
 */
class ChallengeText : public WifiInformationElement
{
  public:
    /**
     * Create ChallengeText
     */
    ChallengeText();
    /**
     * Create ChallengeText from a given string
     *
     * \param s ChallengeText in string
     */
    ChallengeText(std::string s);

    // Implementations of pure virtual methods of WifiInformationElement
    WifiInformationElementId ElementId() const override;
    void Print(std::ostream& os) const override;

    /**
     * Check if the two ChallengeText are equal.
     *
     * \param o ChallengeText to compare to
     *
     * \return true if the two ChallengeText are equal,
     *         false otherwise
     */
    bool IsEqual(const ChallengeText& o) const;

    /**
     * Peek the ChallengeText.
     *
     * \return a pointer to ChallengeText string
     */
    std::string PeekString() const;

    void SetChallengeText(const std::string& text);


  private:
    uint16_t GetInformationFieldSize() const override;
    void SerializeInformationField(Buffer::Iterator start) const override;
    uint16_t DeserializeInformationField(Buffer::Iterator start, uint16_t length) override;

    std::vector<uint8_t> m_challengetext; //!< Raw ChallengeText value
    uint8_t m_length;   //!< Length of the ChallengeText
};

/**
 * Serialize from the given istream to this ChallengeText.
 *
 * \param is the input stream
 * \param ssid the ChallengeText
 *
 * \return std::istream
 */
std::istream& operator>>(std::istream& is, ChallengeText& challenge_text);

ATTRIBUTE_HELPER_HEADER(ChallengeText);

} // namespace ns3

#endif /* CHALLENGETEXT_H */
