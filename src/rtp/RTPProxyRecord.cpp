/*
 * Copyright (C) 2012  OSS Software Solutions
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with main.c; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */


#include <OSS/Core.h>
#include "OSS/RTP/RTPProxyRecord.h"


namespace OSS {
namespace RTP {


RTPProxyRecord::RTPProxyRecord()
{
  timestamp = 0;

  isExpectingInitialAnswer = false;
  hasOfferedAudioProxy = false;
  hasOfferedVideoProxy = false;
  hasOfferedFaxProxy = false;
  isAudioProxyNegotiated = false;
  isVideoProxyNegotiated = false;
  isFaxProxyNegotiated = false;
  verbose = false;
  state = 0;
  lastOfferIndex = 0;

  audio.data.adjustSenderFromPacketSource = false;
  audio.data.leg1Reset = false;
  audio.data.leg2Reset = false;
  audio.data.isStarted = false;
  audio.data.isInactive = false;
  audio.data.isLeg1XOREncrypted = false;
  audio.data.isLeg2XOREncrypted = false;

  audio.control.adjustSenderFromPacketSource = false;
  audio.control.leg1Reset = false;
  audio.control.leg2Reset = false;
  audio.control.isStarted = false;
  audio.control.isInactive = false;
  audio.control.isLeg1XOREncrypted = false;
  audio.control.isLeg2XOREncrypted = false;

  video.data.adjustSenderFromPacketSource = false;
  video.data.leg1Reset = false;
  video.data.leg2Reset = false;
  video.data.isStarted = false;
  video.data.isInactive = false;
  video.data.isLeg1XOREncrypted = false;
  video.data.isLeg2XOREncrypted = false;

  video.control.adjustSenderFromPacketSource = false;
  video.control.leg1Reset = false;
  video.control.leg2Reset = false;
  video.control.isStarted = false;
  video.control.isInactive = false;
  video.control.isLeg1XOREncrypted = false;
  video.control.isLeg2XOREncrypted = false;

  fax.data.adjustSenderFromPacketSource = false;
  fax.data.leg1Reset = false;
  fax.data.leg2Reset = false;
  fax.data.isStarted = false;
  fax.data.isInactive = false;
  fax.data.isLeg1XOREncrypted = false;
  fax.data.isLeg2XOREncrypted = false;

  fax.control.adjustSenderFromPacketSource = false;
  fax.control.leg1Reset = false;
  fax.control.leg2Reset = false;
  fax.control.isStarted = false;
  fax.control.isInactive = false;
  fax.control.isLeg1XOREncrypted = false;
  fax.control.isLeg2XOREncrypted = false;
}

bool RTPProxyRecord::writeToRedis(RedisBroadcastClient& client, const std::string& key) const
{
  json::Object params;

  params["timestamp"] = json::Number(OSS::getTime());
  // strings
  params["identifier"] = json::String(identifier);
  params["logId"] = json::String(logId);
  params["leg1Identifier"] = json::String(leg1Identifier);
  params["leg2Identifier"] = json::String(leg2Identifier);
  params["lastSDPInAck"] = json::String(lastSDPInAck);
  
  // bools
  params["isExpectingInitialAnswer"] = json::Boolean(isExpectingInitialAnswer);
  params["hasOfferedAudioProxy"] = json::Boolean(hasOfferedAudioProxy);
  params["hasOfferedVideoProxy"] = json::Boolean(hasOfferedVideoProxy);
  params["hasOfferedFaxProxy"] = json::Boolean(hasOfferedFaxProxy);
  params["isAudioProxyNegotiated"] = json::Boolean(isAudioProxyNegotiated);
  params["isVideoProxyNegotiated"] = json::Boolean(isVideoProxyNegotiated);
  params["isFaxProxyNegotiated"] = json::Boolean(isFaxProxyNegotiated);
  params["verbose"] = json::Boolean(verbose);

  //ints
  params["state"] = json::Number(state);
  params["lastOfferIndex"] = json::Number(lastOfferIndex);


  if (hasOfferedAudioProxy)
  {
    json::Object audio_data;
    json::Object audio_control;
    // strings
    audio_data["identifier"] = json::String(audio.data.identifier);
    audio_data["localEndPointLeg1"] = json::String(audio.data.localEndPointLeg1);
    audio_data["localEndPointLeg2"] = json::String(audio.data.localEndPointLeg2);
    audio_data["senderEndPointLeg1"] = json::String(audio.data.senderEndPointLeg1);
    audio_data["senderEndPointLeg2"] = json::String(audio.data.senderEndPointLeg2);
    audio_data["lastSenderEndPointLeg1"] = json::String(audio.data.lastSenderEndPointLeg1);
    audio_data["lastSenderEndPointLeg2"] = json::String(audio.data.lastSenderEndPointLeg2);
    //  bools
    audio_data["adjustSenderFromPacketSource"] = json::Boolean(audio.data.adjustSenderFromPacketSource);
    audio_data["leg1Reset"] = json::Boolean(audio.data.leg1Reset);
    audio_data["leg2Reset"] = json::Boolean(audio.data.leg2Reset);
    audio_data["isStarted"] = json::Boolean(audio.data.isStarted);
    audio_data["isInactive"] = json::Boolean(audio.data.isInactive);
    audio_data["isLeg1XOREncrypted"] = json::Boolean(audio.data.isLeg1XOREncrypted);
    audio_data["isLeg2XOREncrypted"] = json::Boolean(audio.data.isLeg2XOREncrypted);
    // strings
    audio_control["identifier"] = json::String(audio.control.identifier);
    audio_control["localEndPointLeg1"] = json::String(audio.control.localEndPointLeg1);
    audio_control["localEndPointLeg2"] = json::String(audio.control.localEndPointLeg2);
    audio_control["senderEndPointLeg1"] = json::String(audio.control.senderEndPointLeg1);
    audio_control["senderEndPointLeg2"] = json::String(audio.control.senderEndPointLeg2);
    audio_control["lastSenderEndPointLeg1"] = json::String(audio.control.lastSenderEndPointLeg1);
    audio_control["lastSenderEndPointLeg2"] = json::String(audio.control.lastSenderEndPointLeg2);
    //  bools
    audio_control["adjustSenderFromPacketSource"] = json::Boolean(audio.control.adjustSenderFromPacketSource);
    audio_control["leg1Reset"] = json::Boolean(audio.control.leg1Reset);
    audio_control["leg2Reset"] = json::Boolean(audio.control.leg2Reset);
    audio_control["isStarted"] = json::Boolean(audio.control.isStarted);
    audio_control["isInactive"] = json::Boolean(audio.control.isInactive);
    audio_control["isLeg1XOREncrypted"] = json::Boolean(audio.control.isLeg1XOREncrypted);
    audio_control["isLeg2XOREncrypted"] = json::Boolean(audio.control.isLeg2XOREncrypted);
    params["audio_data"] = audio_data;
    params["audio_control"] = audio_control;
  }

  if (hasOfferedVideoProxy)
  {
    json::Object video_data;
    json::Object video_control;
    // strings
    video_data["identifier"] = json::String(video.data.identifier);
    video_data["localEndPointLeg1"] = json::String(video.data.localEndPointLeg1);
    video_data["localEndPointLeg2"] = json::String(video.data.localEndPointLeg2);
    video_data["senderEndPointLeg1"] = json::String(video.data.senderEndPointLeg1);
    video_data["senderEndPointLeg2"] = json::String(video.data.senderEndPointLeg2);
    video_data["lastSenderEndPointLeg1"] = json::String(video.data.lastSenderEndPointLeg1);
    video_data["lastSenderEndPointLeg2"] = json::String(video.data.lastSenderEndPointLeg2);
    //  bools
    video_data["adjustSenderFromPacketSource"] = json::Boolean(video.data.adjustSenderFromPacketSource);
    video_data["leg1Reset"] = json::Boolean(video.data.leg1Reset);
    video_data["leg2Reset"] = json::Boolean(video.data.leg2Reset);
    video_data["isStarted"] = json::Boolean(video.data.isStarted);
    video_data["isInactive"] = json::Boolean(video.data.isInactive);
    video_data["isLeg1XOREncrypted"] = json::Boolean(video.data.isLeg1XOREncrypted);
    video_data["isLeg2XOREncrypted"] = json::Boolean(video.data.isLeg2XOREncrypted);
    // strings
    video_control["identifier"] = json::String(video.control.identifier);
    video_control["localEndPointLeg1"] = json::String(video.control.localEndPointLeg1);
    video_control["localEndPointLeg2"] = json::String(video.control.localEndPointLeg2);
    video_control["senderEndPointLeg1"] = json::String(video.control.senderEndPointLeg1);
    video_control["senderEndPointLeg2"] = json::String(video.control.senderEndPointLeg2);
    video_control["lastSenderEndPointLeg1"] = json::String(video.control.lastSenderEndPointLeg1);
    video_control["lastSenderEndPointLeg2"] = json::String(video.control.lastSenderEndPointLeg2);
    //  bools
    video_control["adjustSenderFromPacketSource"] = json::Boolean(video.control.adjustSenderFromPacketSource);
    video_control["leg1Reset"] = json::Boolean(video.control.leg1Reset);
    video_control["leg2Reset"] = json::Boolean(video.control.leg2Reset);
    video_control["isStarted"] = json::Boolean(video.control.isStarted);
    video_control["isInactive"] = json::Boolean(video.control.isInactive);
    video_control["isLeg1XOREncrypted"] = json::Boolean(video.control.isLeg1XOREncrypted);
    video_control["isLeg2XOREncrypted"] = json::Boolean(video.control.isLeg2XOREncrypted);
    params["video_data"] = video_data;
    params["video_control"] = video_control;
  }

  if (hasOfferedFaxProxy)
  {
    json::Object fax_data;
    json::Object fax_control;
    // strings
    fax_data["identifier"] = json::String(fax.data.identifier);
    fax_data["localEndPointLeg1"] = json::String(fax.data.localEndPointLeg1);
    fax_data["localEndPointLeg2"] = json::String(fax.data.localEndPointLeg2);
    fax_data["senderEndPointLeg1"] = json::String(fax.data.senderEndPointLeg1);
    fax_data["senderEndPointLeg2"] = json::String(fax.data.senderEndPointLeg2);
    fax_data["lastSenderEndPointLeg1"] = json::String(fax.data.lastSenderEndPointLeg1);
    fax_data["lastSenderEndPointLeg2"] = json::String(fax.data.lastSenderEndPointLeg2);
    //  bools
    fax_data["adjustSenderFromPacketSource"] = json::Boolean(fax.data.adjustSenderFromPacketSource);
    fax_data["leg1Reset"] = json::Boolean(fax.data.leg1Reset);
    fax_data["leg2Reset"] = json::Boolean(fax.data.leg2Reset);
    fax_data["isStarted"] = json::Boolean(fax.data.isStarted);
    fax_data["isInactive"] = json::Boolean(fax.data.isInactive);
    fax_data["isLeg1XOREncrypted"] = json::Boolean(fax.data.isLeg1XOREncrypted);
    fax_data["isLeg2XOREncrypted"] = json::Boolean(fax.data.isLeg2XOREncrypted);
    // strings
    fax_control["identifier"] = json::String(fax.control.identifier);
    fax_control["localEndPointLeg1"] = json::String(fax.control.localEndPointLeg1);
    fax_control["localEndPointLeg2"] = json::String(fax.control.localEndPointLeg2);
    fax_control["senderEndPointLeg1"] = json::String(fax.control.senderEndPointLeg1);
    fax_control["senderEndPointLeg2"] = json::String(fax.control.senderEndPointLeg2);
    fax_control["lastSenderEndPointLeg1"] = json::String(fax.control.lastSenderEndPointLeg1);
    fax_control["lastSenderEndPointLeg2"] = json::String(fax.control.lastSenderEndPointLeg2);
    //  bools
    fax_control["adjustSenderFromPacketSource"] = json::Boolean(fax.control.adjustSenderFromPacketSource);
    fax_control["leg1Reset"] = json::Boolean(fax.control.leg1Reset);
    fax_control["leg2Reset"] = json::Boolean(fax.control.leg2Reset);
    fax_control["isStarted"] = json::Boolean(fax.control.isStarted);
    fax_control["isInactive"] = json::Boolean(fax.control.isInactive);
    fax_control["isLeg1XOREncrypted"] = json::Boolean(fax.control.isLeg1XOREncrypted);
    fax_control["isLeg2XOREncrypted"] = json::Boolean(fax.control.isLeg2XOREncrypted);
    params["fax_data"] = fax_data;
    params["fax_control"] = fax_control;
  }

  return client.set(key, params, 3600 * 12);
}

bool RTPProxyRecord::readFromRedis(RedisBroadcastClient& client, const std::string& key)
{
  json::Object params;
  if (!client.get(key, params))
    return false;

  json::Number timestamp_ = params["timestamp"]; timestamp = (OSS::UInt64)timestamp_.Value();
  // strings
  json::String identifier_ = params["identifier"]; identifier = identifier_.Value();
  json::String logId_ = params["logId"]; logId = logId_.Value();
  json::String leg1Identifier_ = params["leg1Identifier"]; leg1Identifier = leg1Identifier_.Value();
  json::String leg2Identifier_ = params["leg2Identifier"]; leg2Identifier = leg2Identifier_.Value();
  json::String lastSDPInAck_ = params["lastSDPInAck"]; lastSDPInAck = lastSDPInAck_.Value();

  // bools
  json::Boolean isExpectingInitialAnswer_ = params["isExpectingInitialAnswer"]; isExpectingInitialAnswer = isExpectingInitialAnswer_.Value();
  json::Boolean hasOfferedAudioProxy_ = params["hasOfferedAudioProxy"]; hasOfferedAudioProxy = hasOfferedAudioProxy_.Value();
  json::Boolean hasOfferedVideoProxy_ = params["hasOfferedVideoProxy"]; hasOfferedVideoProxy = hasOfferedVideoProxy_.Value();
  json::Boolean hasOfferedFaxProxy_ = params["hasOfferedFaxProxy"]; hasOfferedFaxProxy = hasOfferedFaxProxy_.Value();
  json::Boolean isAudioProxyNegotiated_ = params["isAudioProxyNegotiated"]; isAudioProxyNegotiated = isAudioProxyNegotiated_.Value();
  json::Boolean isVideoProxyNegotiated_ = params["isVideoProxyNegotiated"]; isVideoProxyNegotiated = isVideoProxyNegotiated_.Value();
  json::Boolean isFaxProxyNegotiated_ = params["isFaxProxyNegotiated"]; isFaxProxyNegotiated = isFaxProxyNegotiated_.Value();
  json::Boolean verbose_ = params["verbose"]; verbose = verbose_.Value();

  //ints
  json::Number state_ = params["state"]; state = state_.Value();
  json::Number lastOfferIndex_ = params["lastOfferIndex"]; lastOfferIndex = lastOfferIndex_.Value();

  if (hasOfferedAudioProxy)
  {
    // objects
    json::Object audio_data = params["audio_data"];
    json::Object audio_control = params["audio_control"];

    //strings
    json::String audio_data_identifier = audio_data["identifier"]; audio.data.identifier = audio_data_identifier.Value();
    json::String audio_data_localEndPointLeg1 = audio_data["localEndPointLeg1"]; audio.data.localEndPointLeg1 = audio_data_localEndPointLeg1.Value();
    json::String audio_data_localEndPointLeg2 = audio_data["localEndPointLeg2"]; audio.data.localEndPointLeg2 = audio_data_localEndPointLeg2.Value();
    json::String audio_data_senderEndPointLeg1 = audio_data["senderEndPointLeg1"]; audio.data.senderEndPointLeg1 = audio_data_senderEndPointLeg1.Value();
    json::String audio_data_senderEndPointLeg2 = audio_data["senderEndPointLeg2"]; audio.data.senderEndPointLeg2 = audio_data_senderEndPointLeg2.Value();
    json::String audio_data_lastSenderEndPointLeg1 = audio_data["lastSenderEndPointLeg1"]; audio.data.lastSenderEndPointLeg1 = audio_data_lastSenderEndPointLeg1.Value();
    json::String audio_data_lastSenderEndPointLeg2 = audio_data["lastSenderEndPointLeg2"]; audio.data.lastSenderEndPointLeg2 = audio_data_lastSenderEndPointLeg2.Value();

    //  bools
    json::Boolean audio_data_adjustSenderFromPacketSource = audio_data["adjustSenderFromPacketSource"]; audio.data.adjustSenderFromPacketSource = audio_data_adjustSenderFromPacketSource.Value();
    json::Boolean audio_data_leg1Reset = audio_data["leg1Reset"]; audio.data.leg1Reset = audio_data_leg1Reset.Value();
    json::Boolean audio_data_leg2Reset = audio_data["leg2Reset"]; audio.data.leg2Reset = audio_data_leg2Reset.Value();
    json::Boolean audio_data_isStarted = audio_data["isStarted"]; audio.data.isStarted = audio_data_isStarted.Value();
    json::Boolean audio_data_isInactive = audio_data["isInactive"]; audio.data.isInactive = audio_data_isInactive.Value();
    json::Boolean audio_data_isLeg1XOREncrypted = audio_data["isLeg1XOREncrypted"]; audio.data.isLeg1XOREncrypted = audio_data_isLeg1XOREncrypted.Value();
    json::Boolean audio_data_isLeg2XOREncrypted = audio_data["isLeg2XOREncrypted"]; audio.data.isLeg2XOREncrypted = audio_data_isLeg2XOREncrypted.Value();

    //strings
    json::String audio_control_identifier = audio_control["identifier"]; audio.control.identifier = audio_control_identifier.Value();
    json::String audio_control_localEndPointLeg1 = audio_control["localEndPointLeg1"]; audio.control.localEndPointLeg1 = audio_control_localEndPointLeg1.Value();
    json::String audio_control_localEndPointLeg2 = audio_control["localEndPointLeg2"]; audio.control.localEndPointLeg2 = audio_control_localEndPointLeg2.Value();
    json::String audio_control_senderEndPointLeg1 = audio_control["senderEndPointLeg1"]; audio.control.senderEndPointLeg1 = audio_control_senderEndPointLeg1.Value();
    json::String audio_control_senderEndPointLeg2 = audio_control["senderEndPointLeg2"]; audio.control.senderEndPointLeg2 = audio_control_senderEndPointLeg2.Value();
    json::String audio_control_lastSenderEndPointLeg1 = audio_control["lastSenderEndPointLeg1"]; audio.control.lastSenderEndPointLeg1 = audio_control_lastSenderEndPointLeg1.Value();
    json::String audio_control_lastSenderEndPointLeg2 = audio_control["lastSenderEndPointLeg2"]; audio.control.lastSenderEndPointLeg2 = audio_control_lastSenderEndPointLeg2.Value();

    //  bools
    json::Boolean audio_control_adjustSenderFromPacketSource = audio_control["adjustSenderFromPacketSource"]; audio.control.adjustSenderFromPacketSource = audio_control_adjustSenderFromPacketSource.Value();
    json::Boolean audio_control_leg1Reset = audio_control["leg1Reset"]; audio.control.leg1Reset = audio_control_leg1Reset.Value();
    json::Boolean audio_control_leg2Reset = audio_control["leg2Reset"]; audio.control.leg2Reset = audio_control_leg2Reset.Value();
    json::Boolean audio_control_isStarted = audio_control["isStarted"]; audio.control.isStarted = audio_control_isStarted.Value();
    json::Boolean audio_control_isInactive = audio_control["isInactive"]; audio.control.isInactive = audio_control_isInactive.Value();
    json::Boolean audio_control_isLeg1XOREncrypted = audio_control["isLeg1XOREncrypted"]; audio.control.isLeg1XOREncrypted = audio_control_isLeg1XOREncrypted.Value();
    json::Boolean audio_control_isLeg2XOREncrypted = audio_control["isLeg2XOREncrypted"]; audio.control.isLeg2XOREncrypted = audio_control_isLeg2XOREncrypted.Value();
  }

  if (hasOfferedVideoProxy)
  {
    // objects
    json::Object video_data = params["video_data"];
    json::Object video_control = params["video_control"];

    //strings
    json::String video_data_identifier = video_data["identifier"]; video.data.identifier = video_data_identifier.Value();
    json::String video_data_localEndPointLeg1 = video_data["localEndPointLeg1"]; video.data.localEndPointLeg1 = video_data_localEndPointLeg1.Value();
    json::String video_data_localEndPointLeg2 = video_data["localEndPointLeg2"]; video.data.localEndPointLeg2 = video_data_localEndPointLeg2.Value();
    json::String video_data_senderEndPointLeg1 = video_data["senderEndPointLeg1"]; video.data.senderEndPointLeg1 = video_data_senderEndPointLeg1.Value();
    json::String video_data_senderEndPointLeg2 = video_data["senderEndPointLeg2"]; video.data.senderEndPointLeg2 = video_data_senderEndPointLeg2.Value();
    json::String video_data_lastSenderEndPointLeg1 = video_data["lastSenderEndPointLeg1"]; video.data.lastSenderEndPointLeg1 = video_data_lastSenderEndPointLeg1.Value();
    json::String video_data_lastSenderEndPointLeg2 = video_data["lastSenderEndPointLeg2"]; video.data.lastSenderEndPointLeg2 = video_data_lastSenderEndPointLeg2.Value();

    //  bools
    json::Boolean video_data_adjustSenderFromPacketSource = video_data["adjustSenderFromPacketSource"]; video.data.adjustSenderFromPacketSource = video_data_adjustSenderFromPacketSource.Value();
    json::Boolean video_data_leg1Reset = video_data["leg1Reset"]; video.data.leg1Reset = video_data_leg1Reset.Value();
    json::Boolean video_data_leg2Reset = video_data["leg2Reset"]; video.data.leg2Reset = video_data_leg2Reset.Value();
    json::Boolean video_data_isStarted = video_data["isStarted"]; video.data.isStarted = video_data_isStarted.Value();
    json::Boolean video_data_isInactive = video_data["isInactive"]; video.data.isInactive = video_data_isInactive.Value();
    json::Boolean video_data_isLeg1XOREncrypted = video_data["isLeg1XOREncrypted"]; video.data.isLeg1XOREncrypted = video_data_isLeg1XOREncrypted.Value();
    json::Boolean video_data_isLeg2XOREncrypted = video_data["isLeg2XOREncrypted"]; video.data.isLeg2XOREncrypted = video_data_isLeg2XOREncrypted.Value();

    //strings
    json::String video_control_identifier = video_control["identifier"]; video.control.identifier = video_control_identifier.Value();
    json::String video_control_localEndPointLeg1 = video_control["localEndPointLeg1"]; video.control.localEndPointLeg1 = video_control_localEndPointLeg1.Value();
    json::String video_control_localEndPointLeg2 = video_control["localEndPointLeg2"]; video.control.localEndPointLeg2 = video_control_localEndPointLeg2.Value();
    json::String video_control_senderEndPointLeg1 = video_control["senderEndPointLeg1"]; video.control.senderEndPointLeg1 = video_control_senderEndPointLeg1.Value();
    json::String video_control_senderEndPointLeg2 = video_control["senderEndPointLeg2"]; video.control.senderEndPointLeg2 = video_control_senderEndPointLeg2.Value();
    json::String video_control_lastSenderEndPointLeg1 = video_control["lastSenderEndPointLeg1"]; video.control.lastSenderEndPointLeg1 = video_control_lastSenderEndPointLeg1.Value();
    json::String video_control_lastSenderEndPointLeg2 = video_control["lastSenderEndPointLeg2"]; video.control.lastSenderEndPointLeg2 = video_control_lastSenderEndPointLeg2.Value();

    //  bools
    json::Boolean video_control_adjustSenderFromPacketSource = video_control["adjustSenderFromPacketSource"]; video.control.adjustSenderFromPacketSource = video_control_adjustSenderFromPacketSource.Value();
    json::Boolean video_control_leg1Reset = video_control["leg1Reset"]; video.control.leg1Reset = video_control_leg1Reset.Value();
    json::Boolean video_control_leg2Reset = video_control["leg2Reset"]; video.control.leg2Reset = video_control_leg2Reset.Value();
    json::Boolean video_control_isStarted = video_control["isStarted"]; video.control.isStarted = video_control_isStarted.Value();
    json::Boolean video_control_isInactive = video_control["isInactive"]; video.control.isInactive = video_control_isInactive.Value();
    json::Boolean video_control_isLeg1XOREncrypted = video_control["isLeg1XOREncrypted"]; video.control.isLeg1XOREncrypted = video_control_isLeg1XOREncrypted.Value();
    json::Boolean video_control_isLeg2XOREncrypted = video_control["isLeg2XOREncrypted"]; video.control.isLeg2XOREncrypted = video_control_isLeg2XOREncrypted.Value();
  }

  if (hasOfferedFaxProxy)
  {
    // objects
    json::Object fax_data = params["fax_data"];
    json::Object fax_control = params["fax_control"];

    //strings
    json::String fax_data_identifier = fax_data["identifier"]; fax.data.identifier = fax_data_identifier.Value();
    json::String fax_data_localEndPointLeg1 = fax_data["localEndPointLeg1"]; fax.data.localEndPointLeg1 = fax_data_localEndPointLeg1.Value();
    json::String fax_data_localEndPointLeg2 = fax_data["localEndPointLeg2"]; fax.data.localEndPointLeg2 = fax_data_localEndPointLeg2.Value();
    json::String fax_data_senderEndPointLeg1 = fax_data["senderEndPointLeg1"]; fax.data.senderEndPointLeg1 = fax_data_senderEndPointLeg1.Value();
    json::String fax_data_senderEndPointLeg2 = fax_data["senderEndPointLeg2"]; fax.data.senderEndPointLeg2 = fax_data_senderEndPointLeg2.Value();
    json::String fax_data_lastSenderEndPointLeg1 = fax_data["lastSenderEndPointLeg1"]; fax.data.lastSenderEndPointLeg1 = fax_data_lastSenderEndPointLeg1.Value();
    json::String fax_data_lastSenderEndPointLeg2 = fax_data["lastSenderEndPointLeg2"]; fax.data.lastSenderEndPointLeg2 = fax_data_lastSenderEndPointLeg2.Value();

    //  bools
    json::Boolean fax_data_adjustSenderFromPacketSource = fax_data["adjustSenderFromPacketSource"]; fax.data.adjustSenderFromPacketSource = fax_data_adjustSenderFromPacketSource.Value();
    json::Boolean fax_data_leg1Reset = fax_data["leg1Reset"]; fax.data.leg1Reset = fax_data_leg1Reset.Value();
    json::Boolean fax_data_leg2Reset = fax_data["leg2Reset"]; fax.data.leg2Reset = fax_data_leg2Reset.Value();
    json::Boolean fax_data_isStarted = fax_data["isStarted"]; fax.data.isStarted = fax_data_isStarted.Value();
    json::Boolean fax_data_isInactive = fax_data["isInactive"]; fax.data.isInactive = fax_data_isInactive.Value();
    json::Boolean fax_data_isLeg1XOREncrypted = fax_data["isLeg1XOREncrypted"]; fax.data.isLeg1XOREncrypted = fax_data_isLeg1XOREncrypted.Value();
    json::Boolean fax_data_isLeg2XOREncrypted = fax_data["isLeg2XOREncrypted"]; fax.data.isLeg2XOREncrypted = fax_data_isLeg2XOREncrypted.Value();

    //strings
    json::String fax_control_identifier = fax_control["identifier"]; fax.control.identifier = fax_control_identifier.Value();
    json::String fax_control_localEndPointLeg1 = fax_control["localEndPointLeg1"]; fax.control.localEndPointLeg1 = fax_control_localEndPointLeg1.Value();
    json::String fax_control_localEndPointLeg2 = fax_control["localEndPointLeg2"]; fax.control.localEndPointLeg2 = fax_control_localEndPointLeg2.Value();
    json::String fax_control_senderEndPointLeg1 = fax_control["senderEndPointLeg1"]; fax.control.senderEndPointLeg1 = fax_control_senderEndPointLeg1.Value();
    json::String fax_control_senderEndPointLeg2 = fax_control["senderEndPointLeg2"]; fax.control.senderEndPointLeg2 = fax_control_senderEndPointLeg2.Value();
    json::String fax_control_lastSenderEndPointLeg1 = fax_control["lastSenderEndPointLeg1"]; fax.control.lastSenderEndPointLeg1 = fax_control_lastSenderEndPointLeg1.Value();
    json::String fax_control_lastSenderEndPointLeg2 = fax_control["lastSenderEndPointLeg2"]; fax.control.lastSenderEndPointLeg2 = fax_control_lastSenderEndPointLeg2.Value();

    //  bools
    json::Boolean fax_control_adjustSenderFromPacketSource = fax_control["adjustSenderFromPacketSource"]; fax.control.adjustSenderFromPacketSource = fax_control_adjustSenderFromPacketSource.Value();
    json::Boolean fax_control_leg1Reset = fax_control["leg1Reset"]; fax.control.leg1Reset = fax_control_leg1Reset.Value();
    json::Boolean fax_control_leg2Reset = fax_control["leg2Reset"]; fax.control.leg2Reset = fax_control_leg2Reset.Value();
    json::Boolean fax_control_isStarted = fax_control["isStarted"]; fax.control.isStarted = fax_control_isStarted.Value();
    json::Boolean fax_control_isInactive = fax_control["isInactive"]; fax.control.isInactive = fax_control_isInactive.Value();
    json::Boolean fax_control_isLeg1XOREncrypted = fax_control["isLeg1XOREncrypted"]; fax.control.isLeg1XOREncrypted = fax_control_isLeg1XOREncrypted.Value();
    json::Boolean fax_control_isLeg2XOREncrypted = fax_control["isLeg2XOREncrypted"]; fax.control.isLeg2XOREncrypted = fax_control_isLeg2XOREncrypted.Value();
  }
  
  return true;
}



} } // OSS::RTP



