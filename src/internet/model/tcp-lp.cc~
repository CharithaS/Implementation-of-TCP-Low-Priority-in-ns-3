/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "tcp-lp.h"
#include "ns3/tcp-socket-base.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TcpLp");
NS_OBJECT_ENSURE_REGISTERED (TcpLp);

TypeId
TcpLp::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpLp")
    .SetParent<TcpNewReno> ()
    .AddConstructor<TcpLp> ()
    .SetGroupName ("Internet")
  ;
  return tid;
}

TcpLp::TcpLp (void)
  : TcpNewReno (),
    m_flag (0),
    m_sowd (0),
    m_owdMin (0xffffffff),
    m_owdMax (0),
    m_owdMaxRsv (0),
    m_lastDrop (0),
    m_inference (0)
{
  NS_LOG_FUNCTION (this);
}

TcpLp::TcpLp (const TcpLp& sock)
  : TcpNewReno (sock),
    m_flag (sock.m_flag),
    m_sowd (sock.m_sowd),
    m_owdMin (sock.m_owdMin),
    m_owdMax (sock.m_owdMax),
    m_owdMaxRsv (sock.m_owdMaxRsv),
    m_lastDrop (sock.m_lastDrop),
    m_inference (sock.m_inference)
{
  NS_LOG_FUNCTION (this);
}

TcpLp::~TcpLp (void)
{
  NS_LOG_FUNCTION (this);
}

Ptr<TcpCongestionOps>
TcpLp::Fork (void)
{
  return CopyObject<TcpLp> (this);
}

void
TcpLp::CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
  NS_LOG_FUNCTION (this << tcb << segmentsAcked);

  if (!(m_flag & LP_WITHIN_INF))
    {
      TcpNewReno::CongestionAvoidance (tcb,segmentsAcked);
    }
}

uint32_t
TcpLp::OWDCalculator (Ptr<TcpSocketState> tcb)
{
  uint32_t owd = 0;
   owd =
    tcb->m_rcvtsval -
    tcb->m_rcvtsecr;
  if (owd < 0)
    {
      owd = -owd;
    }
  
  if (owd > 0)
    {
      m_flag |= LP_VALID_OWD;
    }
  else
    {
      m_flag &= ~LP_VALID_OWD;
    }

  return owd;

}

void
TcpLp::RttSample (Ptr<TcpSocketState> tcb)
{
  uint32_t mowd = OWDCalculator (tcb);

  if (!(m_flag & LP_VALID_OWD))
    {
      return;
    }

  /* record the next min owd */
  if (mowd < m_owdMin)
    {
      m_owdMin = mowd;
    }

  if (mowd > m_owdMax)
    {
      if (mowd > m_owdMaxRsv)
        {
          if (m_owdMaxRsv == 0)
            {
              m_owdMax = mowd;
            }
          else
            {
              m_owdMax = m_owdMaxRsv;
            }
          m_owdMaxRsv = mowd;
        }
      else
        {

          m_owdMax = mowd;
        }
    }

  /* calc for smoothed owd */
  if (m_sowd != 0)
    {
      mowd -= m_sowd >> 3;              /* m is now error in owd est */
      m_sowd += mowd;           /* owd = 7/8 owd + 1/8 new */
    }
  else
    {
      m_sowd = mowd << 3;               /* take the measured time be owd */
    }
}


void
TcpLp::PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked,
                  const Time &rtt)
{
  segmentsAcked = segmentsAcked;

  if (!rtt.IsZero ())
    {
      RttSample (tcb);
    }

  uint64_t timestamp = (uint64_t) Simulator::Now ().GetSeconds ();
  /* calc inference */
  if (timestamp > tcb->m_rcvtsecr)
    {
      m_inference = 3 * (timestamp - tcb->m_rcvtsecr);
    }

  /* test if within inference */
  if (m_lastDrop && (timestamp - m_lastDrop < m_inference))
    {
      m_flag |= LP_WITHIN_INF;
    }
  else
    {
      m_flag &= ~LP_WITHIN_INF;
    }

  /* test if within threshold */
  if (m_sowd >> 3 <=
      m_owdMin + 15 * (m_owdMax - m_owdMin) / 100)
    {
      m_flag |= LP_WITHIN_THR;
    }
  else
    {
      m_flag &= ~LP_WITHIN_THR;
    }

  if (m_flag & LP_WITHIN_THR)
    {
      return;
    }


  m_owdMin = m_sowd >> 3;
  m_owdMax = m_sowd >> 2;
  m_owdMaxRsv = m_sowd >> 2;

  /* happened within inference
   * drop snd_cwnd into 1 */
  if (m_flag & LP_WITHIN_INF)
    {
      tcb->m_cWnd = 1U * tcb->m_segmentSize;
    }

  /* happened after inference
   * cut snd_cwnd into half */
  else
    {
      tcb->m_cWnd = std::max (tcb->m_cWnd.Get () >> 1U, 1U * tcb->m_segmentSize);
    }

  /* record this drop time */
  m_lastDrop = timestamp;
}

std::string
TcpLp::GetName () const
{
  return "TcpLp";
}
} // namespace ns3
