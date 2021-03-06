/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#ifndef WIFI_PHY_STATE_HELPER_H
#define WIFI_PHY_STATE_HELPER_H

#include "ns3/object.h"
#include "ns3/callback.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "wifi-phy-state.h"
#include "wifi-preamble.h"
#include "wifi-ppdu.h"
#include "ns3/wifi-spectrum-value-helper.h"

namespace ns3 {

class WifiPhyListener;
class WifiTxVector;
class WifiMode;
class Packet;
class WifiPsdu;
struct RxSignalInfo;

/**
 * Callback if PSDU successfully received (i.e. if aggregate,
 * it means that at least one MPDU of the A-MPDU was received,
 * considering that the per-MPDU reception status is also provided).
 *
 * arg1: PSDU received successfully
 * arg2: info on the received signal (\see RxSignalInfo)
 * arg3: TXVECTOR of PSDU
 * arg4: vector of per-MPDU status of reception.
 */
typedef Callback<void, Ptr<WifiPsdu>, RxSignalInfo,
                       WifiTxVector, std::vector<bool>> RxOkCallback;
/**
 * Callback if PSDU unsuccessfully received
 *
 * arg1: PSDU received unsuccessfully
 */
typedef Callback<void, Ptr<WifiPsdu>> RxErrorCallback;

/**
 * \ingroup wifi
 *
 * This objects implements the PHY state machine of the Wifi device.
 */
class WifiPhyStateHelper : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  WifiPhyStateHelper ();
  virtual ~WifiPhyStateHelper ();

  /**
   * Set a callback for a successful reception.
   *
   * \param callback
   */
  void SetReceiveOkCallback (RxOkCallback callback);
  /**
   * Set a callback for a failed reception.
   *
   * \param callback
   */
  void SetReceiveErrorCallback (RxErrorCallback callback);
  /**
   * Register WifiPhyListener to this WifiPhyStateHelper.
   *
   * \param listener
   */
  void RegisterListener (WifiPhyListener *listener);
  /**
   * Remove WifiPhyListener from this WifiPhyStateHelper.
   *
   * \param listener
   */
  void UnregisterListener (WifiPhyListener *listener);
  /**
   * Return the current state of WifiPhy.
   *
   * \param band the band that corresponds to the channel to check
   * \param ccaThreshold the threshold used to determine whether the band is CCA_BUSY
   *
   * \return the current state of WifiPhy
   */
  WifiPhyState GetState (WifiSpectrumBand band, double ccaThreshold) const;
  /**
   * Check whether the current state is CCA busy.
   *
   * \param band the band that corresponds to the channel to check
   * \param ccaThreshold the threshold used to determine whether the band is CCA_BUSY
   *
   * \return true if the current state is CCA busy, false otherwise
   */
  bool IsStateCcaBusy (WifiSpectrumBand band, double ccaThreshold) const;
  /**
   * Check whether the current state is IDLE.
   *
   * \return true if the current state is IDLE, false otherwise
   */
  bool IsStateIdle (WifiSpectrumBand band, double ccaThreshold) const;
  /**
   * Check whether the current state is RX.
   *
   * \param band the band that corresponds to the channel to check
   * \param ccaThreshold the threshold used to determine whether the band is CCA_BUSY
   *
   * \return true if the current state is RX, false otherwise
   */
  bool IsStateRx (WifiSpectrumBand primaryBand, double primaryCcaThreshold) const;
  /**
   * Check whether the current state is TX.
   *
   * \param primaryBand the band that corresponds to the primary channel
   * \param primaryCcaThreshold the threshold used to determine whether the primary channel is CCA_BUSY
   *
   * \return true if the current state is TX, false otherwise
   */
  bool IsStateTx (WifiSpectrumBand primaryBand, double primaryCcaThreshold) const;
  /**
   * Check whether the current state is SWITCHING.
   *
   * \param primaryBand the band that corresponds to the primary channel
   * \param primaryCcaThreshold the threshold used to determine whether the primary channel is CCA_BUSY
   *
   * \return true if the current state is SWITCHING, false otherwise
   */
  bool IsStateSwitching (WifiSpectrumBand primaryBand, double primaryCcaThreshold) const;
  /**
   * Check whether the current state is SLEEP.
   *
   * \param primaryBand the band that corresponds to the primary channel
   * \param primaryCcaThreshold the threshold used to determine whether the primary channel is CCA_BUSY
   *
   * \return true if the current state is SLEEP, false otherwise
   */
  bool IsStateSleep (WifiSpectrumBand primaryBand, double primaryCcaThreshold) const;
  /**
   * Check whether the current state is OFF.
   *
   * \param primaryBand the band that corresponds to the primary channel
   * \param primaryCcaThreshold the threshold used to determine whether the primary channel is CCA_BUSY
   *
   * \return true if the current state is OFF, false otherwise
   */
  bool IsStateOff (WifiSpectrumBand primaryBand, double primaryCcaThreshold) const;
  /**
   * Return the time before the state is back to IDLE.
   *
   * \param band the band that corresponds to the channel to check
   * \param ccaThreshold the threshold used to determine whether the band is CCA_BUSY
   *
   * \return the delay before the state is back to IDLE
   */
  Time GetDelayUntilIdle (WifiSpectrumBand band, double ccaThreshold) const;
  /**
   * Return the time since the secondary channel is determined idle.
   *
   * \param band the band that corresponds to the secondary to check
   * \param ccaThreshold the threshold used to determine whether the band is CCA_BUSY
   *
   * \return the delay since the secondary channel is determined idle
   */
  Time GetDelaySinceIdle (WifiSpectrumBand band, double ccaThreshold) const;
  /**
   * Return the time the last RX start.
   *
   * \return the time the last RX start.
   */
  Time GetLastRxStartTime (void) const;

  /**
   * Switch state to TX for the given duration.
   *
   * \param txDuration the duration of the PPDU to transmit
   * \param psdus the PSDUs in the transmitted PPDU (only one unless it is a MU PPDU)
   * \param txPowerDbm the nominal tx power in dBm
   * \param txVector the tx vector for the transmission
   * \param primaryBand the band that corresponds to the primary channel
   * \param primaryCcaThreshold the threshold used to determine whether the primary channel is CCA_BUSY
   */
  void SwitchToTx (Time txDuration, WifiPsduMap psdus, double txPowerDbm, WifiTxVector txVector, WifiSpectrumBand primaryBand, double primaryCcaThreshold);
  /**
   * Switch state to RX for the given duration.
   *
   * \param rxDuration the duration of the RX
   * \param primaryBand the band that corresponds to the primary channel
   * \param primaryCcaThreshold the threshold used to determine whether the primary channel is CCA_BUSY
   */
  void SwitchToRx (Time rxDuration, WifiSpectrumBand primaryBand, double primaryCcaThreshold);
  /**
   * Switch state to channel switching for the given duration.
   *
   * \param switchingDuration the duration of required to switch the channel
   * \param primaryBand the band that corresponds to the primary channel
   * \param primaryCcaThreshold the threshold used to determine whether the primary channel is CCA_BUSY
   */
  void SwitchToChannelSwitching (Time switchingDuration, WifiSpectrumBand primaryBand, double primaryCcaThreshold);
  /**
   * Continue RX after the reception of an MPDU in an A-MPDU was successful.
   *
   * \param psdu the successfully received PSDU
   * \param rxSignalInfo the info on the received signal (\see RxSignalInfo)
   * \param txVector TXVECTOR of the PSDU
   */
  void ContinueRxNextMpdu (Ptr<WifiPsdu> psdu, RxSignalInfo rxSignalInfo, WifiTxVector txVector);
  /**
   * Switch from RX after the reception was successful.
   *
   * \param psdu the successfully received PSDU
   * \param rxSignalInfo the info on the received signal (\see RxSignalInfo)
   * \param txVector TXVECTOR of the PSDU
   * \param staId the station ID of the PSDU (only used for MU)
   * \param statusPerMpdu reception status per MPDU
   */
  void SwitchFromRxEndOk (Ptr<WifiPsdu> psdu, RxSignalInfo rxSignalInfo, WifiTxVector txVector,
                          uint16_t staId, std::vector<bool> statusPerMpdu);
  /**
   * Switch from RX after the reception failed.
   *
   * \param psdu the PSDU that we failed to received
   * \param snr the SNR of the received PSDU
   */
  void SwitchFromRxEndError (Ptr<WifiPsdu> psdu, double snr);
  /**
   * Switch to CCA busy.
   *
   * \param duration the duration of CCA busy state
   * \param band the band for which the CCA busy state is triggered
   * \param isPrimaryChannel flag whether the band corresponds to the primary channel
   * \param ccaThreshold the threshold used to determine the channel is in CCA busy state
   */
  void SwitchMaybeToCcaBusy (Time duration, WifiSpectrumBand band, bool isPrimaryChannel, double ccaThreshold);
  /**
   * Switch to sleep mode.
   *
   * \param primaryBand the band that corresponds to the primary channel
   * \param primaryCcaThreshold the threshold used to determine whether the primary channel is CCA_BUSY
   */
  void SwitchToSleep (WifiSpectrumBand primaryBand, double primaryCcaThreshold);
  /**
   * Switch from sleep mode.
   *
   * \param duration the duration of CCA busy state
   */
  void SwitchFromSleep (Time duration, WifiSpectrumBand band, bool isPrimaryChannel, double ccaThreshold);
  /**
   * Abort current reception
   *
   * \param failure flag to indicate whether RX abortion is due to a failure
   */
  void SwitchFromRxAbort (bool failure);
  /**
   * Switch to off mode.
   *
   * \param primaryBand the band that corresponds to the primary channel
   * \param primaryCcaThreshold the threshold used to determine whether the primary channel is CCA_BUSY
   * \param primaryBand identifies the primary channel
   */
  void SwitchToOff (WifiSpectrumBand primaryBand, double primaryCcaThreshold);
  /**
   * Switch from off mode.
   *
   * \param duration the duration of CCA busy state
   */
  void SwitchFromOff (Time duration, WifiSpectrumBand band, bool isPrimaryChannel, double ccaThreshold);

  /**
   * TracedCallback signature for state changes.
   *
   * \param [in] start Time when the \p state started.
   * \param [in] duration Amount of time we've been in (or will be in)
   *             the \p state.
   * \param [in] state The state.
   */
  typedef void (* StateTracedCallback)(Time start, Time duration, WifiPhyState state);

  /**
   * TracedCallback signature for receive end ok event.
   *
   * \param [in] packet The received packet.
   * \param [in] snr    The SNR of the received packet.
   * \param [in] mode   The transmission mode of the packet.
   * \param [in] preamble The preamble of the packet.
   */
  typedef void (* RxOkTracedCallback)(Ptr<const Packet> packet, double snr, WifiMode mode, WifiPreamble preamble);

  /**
   * TracedCallback signature for receive end error event.
   *
   * \param [in] packet       The received packet.
   * \param [in] snr          The SNR of the received packet.
   */
  typedef void (* RxEndErrorTracedCallback)(Ptr<const Packet> packet, double snr);

  /**
   * TracedCallback signature for transmit event.
   *
   * \param [in] packet The received packet.
   * \param [in] mode   The transmission mode of the packet.
   * \param [in] preamble The preamble of the packet.
   * \param [in] power  The transmit power level.
   */
  typedef void (* TxTracedCallback)(Ptr<const Packet> packet, WifiMode mode,
                                    WifiPreamble preamble, uint8_t power);


protected:
  // Inherited
  virtual void DoInitialize (void);
  virtual void DoDispose (void);


private:
  /**
   * typedef for a list of WifiPhyListeners
   */
  typedef std::vector<WifiPhyListener *> Listeners;
  /**
   * typedef for a list of WifiPhyListeners iterator
   */
  typedef std::vector<WifiPhyListener *>::iterator ListenersI;

  /**
   * Log the ideal and CCA states.
   *
   * \param primaryBand the band that corresponds to the primary channel
   * \param primaryCcaThreshold the threshold used to determine whether the primary channel is CCA_BUSY
   */
  void LogPreviousIdleAndCcaBusyStates (WifiSpectrumBand primaryBand, double primaryCcaThreshold);

  /**
   * Notify all WifiPhyListener that the transmission has started for the given duration.
   *
   * \param duration the duration of the transmission
   * \param txPowerDbm the nominal tx power in dBm
   */
  void NotifyTxStart (Time duration, double txPowerDbm);
  /**
   * Notify all WifiPhyListener that the reception has started for the given duration.
   *
   * \param duration the duration of the reception
   */
  void NotifyRxStart (Time duration);
  /**
   * Notify all WifiPhyListener that the reception was successful.
   */
  void NotifyRxEndOk (void);
  /**
   * Notify all WifiPhyListener that the reception was not successful.
   */
  void NotifyRxEndError (void);
  /**
   * Notify all WifiPhyListener that the CCA has started for the given duration.
   *
   * \param duration the duration of the CCA state
   */
  void NotifyMaybeCcaBusyStart (Time duration);
  /**
   * Notify all WifiPhyListener that we are switching channel with the given channel
   * switching delay.
   *
   * \param duration the delay to switch the channel
   */
  void NotifySwitchingStart (Time duration);
  /**
   * Notify all WifiPhyListener that we are going to sleep
   */
  void NotifySleep (void);
  /**
   * Notify all WifiPhyListener that we are going to switch off
   */
  void NotifyOff (void);
  /**
   * Notify all WifiPhyListener that we woke up
   */
  void NotifyWakeup (void);
  /**
   * Switch the state from RX.
   */
  void DoSwitchFromRx (void);
  /**
   * Notify all WifiPhyListener that we are going to switch on
   */
  void NotifyOn (void);

  /**
   * The trace source fired when state is changed.
   */
  TracedCallback<Time, Time, WifiPhyState> m_stateLogger;

  bool m_sleeping; ///< sleeping
  bool m_isOff; ///< switched off
  Time m_endTx; ///< end transmit
  Time m_endRx; ///< end receive
  Time m_endSwitching; ///< end switching
  Time m_startTx; ///< start transmit
  Time m_startRx; ///< start receive
  Time m_startSwitching; ///< start switching
  Time m_startSleep; ///< start sleep
  Time m_previousStateChangeTime; ///< previous state change time

  std::map<std::pair <WifiSpectrumBand, int /* threshold */>, Time> m_startCcaBusy; ///< start CCA busy per channel
  std::map<std::pair <WifiSpectrumBand, int /* threshold */>, Time> m_endCcaBusy; ///< end CCA busy per channel
  //FIXME: thresholds should be double, but we need to implement std::find differently, so OK to round to int for now

  Listeners m_listeners; ///< listeners
  TracedCallback<Ptr<const Packet>, double, WifiMode, WifiPreamble> m_rxOkTrace; ///< receive OK trace callback
  TracedCallback<Ptr<const Packet>, double> m_rxErrorTrace; ///< receive error trace callback
  TracedCallback<Ptr<const Packet>, WifiMode, WifiPreamble, uint8_t> m_txTrace; ///< transmit trace callback
  RxOkCallback m_rxOkCallback; ///< receive OK callback
  RxErrorCallback m_rxErrorCallback; ///< receive error callback
};

} //namespace ns3

#endif /* WIFI_PHY_STATE_HELPER_H */
