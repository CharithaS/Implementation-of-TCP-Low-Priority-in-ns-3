#ifndef TCPLP_H
#define TCPLP_H

#include "ns3/tcp-congestion-ops.h"
#include "ns3/traced-value.h"

namespace ns3 {

class TcpLp : public TcpNewReno
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Create an unbound tcp socket.
   */
  TcpLp (void);

  /**
   * \brief Copy constructor
   * \param sock the object to copy
   */
  TcpLp (const TcpLp& sock);

  virtual ~TcpLp (void);

  virtual void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                          const Time& rtt);

  virtual std::string GetName () const;

  virtual Ptr<TcpCongestionOps> Fork ();

  virtual void CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
private:
  enum TcpLpState
  {
    LP_VALID_OWD = (1 << 1),
    LP_WITHIN_THR = (1 << 3),
    LP_WITHIN_INF = (1 << 4),
  };
  uint32_t            m_flag;
  uint32_t            m_sowd;
  uint32_t            m_owdMin;
  uint32_t            m_owdMax;
  uint32_t            m_owdMaxRsv;
  uint32_t            m_lastDrop;
  uint32_t            m_inference;

private:
  uint32_t OWDCalculator (Ptr<TcpSocketState> tcb);
  void RttSample (Ptr<TcpSocketState> tcb);
};

} // namespace ns3

#endif // TCPLP_H
