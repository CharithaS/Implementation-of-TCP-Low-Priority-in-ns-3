/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Charitha Sangaraju <charitha29193@gmail.com>
 *          Nandita G <gm.nandita@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 *
 */
 
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
    m_remoteHz (0),
    m_remoteRefTime (0),
    m_localRefTime (0),
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
    m_remoteHz (sock.m_remoteHz),
    m_remoteRefTime (sock.m_remoteRefTime),
    m_localRefTime (sock.m_localRefTime),
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



/*uint32_t
TcpLp::RemoteHzEstimator(Ptr<TcpSocketState> tcb)
{
        int32_t rhz = m_remote_hz << 6;
        int32_t m = 0;

         not yet record reference time
          go away!! record it before come back!!
        if (m_remote_ref_time == 0 || m_local_ref_time == 0)
                goto out;

         we can't calc remote HZ with no different!!
        if (tp->rx_opt.rcv_tsval == m_remote_ref_time
            || tp->rx_opt.rcv_tsecr == m_local_ref_time)
                goto out;

        m = HZ * (tp->rx_opt.rcv_tsval -
                  m_remote_ref_time) / (tp->rx_opt.rcv_tsecr -
                                          m_local_ref_time);
        if (m < 0)
                m = -m;

        if (rhz > 0) {
                m -= rhz >> 6;	 m is now error in remote HZ est
                rhz += m;	 63/64 old + 1/64 new
        } else
                rhz = m << 6;

 out:
         record time for successful remote HZ calc
        if ((rhz >> 6) > 0)
                m_flag |= LP_VALID_RHZ;
        else
                m_flag &= ~LP_VALID_RHZ;

         record reference time stamp
        m_remote_ref_time = tp->rx_opt.rcv_tsval;
        m_local_ref_time = tp->rx_opt.rcv_tsecr;

        return rhz >> 6;
}*/


uint32_t
TcpLp::OWDCalculator (Ptr<TcpSocketState> tcb)
{
  int32_t owd = 0;
  //m_remote_hz = RemoteHzEstimator(tcb);
  if (m_flag & LP_VALID_RHZ)
    {
      owd =
        tcb->m_rcvtsval -
        tcb->m_rcvtsecr;
      if (owd < 0)
        {
          owd = -owd;
        }
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
  int32_t mowd = OWDCalculator (tcb);

  if (!(m_flag & LP_VALID_RHZ) || !(m_flag & LP_VALID_OWD))
    {
      return;
    }

  /* record the next min owd */
  if (mowd < (int32_t)m_owdMin)
    {
      m_owdMin = mowd;
    }

  if (mowd > (int32_t)m_owdMax)
    {
      if (mowd > (int32_t)m_owdMaxRsv)
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
  if (m_sowd >> 3 <
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
