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
  uint32_t            m_owdMin;
  uint32_t            m_owdMax;
  uint32_t            m_owdMaxRsv;
  uint32_t            m_remoteHz;
  uint32_t            m_remoteRefTime;
  uint32_t            m_localRefTime;
  uint32_t            m_lastDrop;
  uint32_t            m_inference;
private:
  uint32_t OwdCalculator (Ptr<TcpSocketState> tcb);
  void RttSample (Ptr<TcpSocketState> tcb);
  //uint32_t RemoteHzEstimator(Ptr<TcpSocketState> tcb);
};

} // namespace ns3

#endif // TCPLP_H
