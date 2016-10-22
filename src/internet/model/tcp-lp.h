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

protected:
  virtual void CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
private:
  enum TcpLpState
  {
    LP_VALID_RHZ = (1 << 0),
    LP_VALID_OWD = (1 << 1),
    LP_WITHIN_THR = (1 << 3),
    LP_WITHIN_INF = (1 << 4),
  };
  uint32_t            m_flag;
  uint32_t            m_sowd;
  uint32_t            m_owd_min;
  uint32_t            m_owd_max;
  uint32_t            m_owd_max_rsv;
  uint32_t            m_remote_hz;
  uint32_t            m_remote_ref_time;
  uint32_t            m_local_ref_time;
  uint32_t            m_last_drop;
  uint32_t            m_inference;

private:
  uint32_t OWDCalculator (Ptr<TcpSocketState> tcb);
  void RttSample (Ptr<TcpSocketState> tcb, uint32_t rtt);
  //uint32_t RemoteHzEstimator(Ptr<TcpSocketState> tcb);
};

} // namespace ns3

#endif // TCPLP_H
