#pragma once
// MESSAGE PEER_POSITION PACKING

#define MAVLINK_MSG_ID_PEER_POSITION 369


typedef struct __mavlink_peer_position_t {
 uint32_t time_boot_ms; /*< [ms] Timestamp (time since system boot).*/
 int32_t lat; /*< [degE7] Latitude, expressed*/
 int32_t lon; /*< [degE7] Longitude, expressed*/
 int32_t alt; /*< [mm] Altitude (MSL). Note that virtually all GPS modules provide both WGS84 and MSL.*/
 int32_t relative_alt; /*< [mm] Altitude above ground*/
 uint16_t hdg; /*< [cdeg] Vehicle heading (yaw angle), 0.0..359.99 degrees. If unknown, set to: UINT16_MAX*/
 uint8_t peer_id; /*<  system's id*/
 uint8_t static_pos; /*<  vehicle positon is static or dynamic*/
} mavlink_peer_position_t;

#define MAVLINK_MSG_ID_PEER_POSITION_LEN 24
#define MAVLINK_MSG_ID_PEER_POSITION_MIN_LEN 24
#define MAVLINK_MSG_ID_369_LEN 24
#define MAVLINK_MSG_ID_369_MIN_LEN 24

#define MAVLINK_MSG_ID_PEER_POSITION_CRC 235
#define MAVLINK_MSG_ID_369_CRC 235



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_PEER_POSITION { \
    369, \
    "PEER_POSITION", \
    8, \
    {  { "time_boot_ms", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_peer_position_t, time_boot_ms) }, \
         { "peer_id", NULL, MAVLINK_TYPE_UINT8_T, 0, 22, offsetof(mavlink_peer_position_t, peer_id) }, \
         { "static_pos", NULL, MAVLINK_TYPE_UINT8_T, 0, 23, offsetof(mavlink_peer_position_t, static_pos) }, \
         { "lat", NULL, MAVLINK_TYPE_INT32_T, 0, 4, offsetof(mavlink_peer_position_t, lat) }, \
         { "lon", NULL, MAVLINK_TYPE_INT32_T, 0, 8, offsetof(mavlink_peer_position_t, lon) }, \
         { "alt", NULL, MAVLINK_TYPE_INT32_T, 0, 12, offsetof(mavlink_peer_position_t, alt) }, \
         { "relative_alt", NULL, MAVLINK_TYPE_INT32_T, 0, 16, offsetof(mavlink_peer_position_t, relative_alt) }, \
         { "hdg", NULL, MAVLINK_TYPE_UINT16_T, 0, 20, offsetof(mavlink_peer_position_t, hdg) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_PEER_POSITION { \
    "PEER_POSITION", \
    8, \
    {  { "time_boot_ms", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_peer_position_t, time_boot_ms) }, \
         { "peer_id", NULL, MAVLINK_TYPE_UINT8_T, 0, 22, offsetof(mavlink_peer_position_t, peer_id) }, \
         { "static_pos", NULL, MAVLINK_TYPE_UINT8_T, 0, 23, offsetof(mavlink_peer_position_t, static_pos) }, \
         { "lat", NULL, MAVLINK_TYPE_INT32_T, 0, 4, offsetof(mavlink_peer_position_t, lat) }, \
         { "lon", NULL, MAVLINK_TYPE_INT32_T, 0, 8, offsetof(mavlink_peer_position_t, lon) }, \
         { "alt", NULL, MAVLINK_TYPE_INT32_T, 0, 12, offsetof(mavlink_peer_position_t, alt) }, \
         { "relative_alt", NULL, MAVLINK_TYPE_INT32_T, 0, 16, offsetof(mavlink_peer_position_t, relative_alt) }, \
         { "hdg", NULL, MAVLINK_TYPE_UINT16_T, 0, 20, offsetof(mavlink_peer_position_t, hdg) }, \
         } \
}
#endif

/**
 * @brief Pack a peer_position message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_boot_ms [ms] Timestamp (time since system boot).
 * @param peer_id  system's id
 * @param static_pos  vehicle positon is static or dynamic
 * @param lat [degE7] Latitude, expressed
 * @param lon [degE7] Longitude, expressed
 * @param alt [mm] Altitude (MSL). Note that virtually all GPS modules provide both WGS84 and MSL.
 * @param relative_alt [mm] Altitude above ground
 * @param hdg [cdeg] Vehicle heading (yaw angle), 0.0..359.99 degrees. If unknown, set to: UINT16_MAX
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_peer_position_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint32_t time_boot_ms, uint8_t peer_id, uint8_t static_pos, int32_t lat, int32_t lon, int32_t alt, int32_t relative_alt, uint16_t hdg)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_PEER_POSITION_LEN];
    _mav_put_uint32_t(buf, 0, time_boot_ms);
    _mav_put_int32_t(buf, 4, lat);
    _mav_put_int32_t(buf, 8, lon);
    _mav_put_int32_t(buf, 12, alt);
    _mav_put_int32_t(buf, 16, relative_alt);
    _mav_put_uint16_t(buf, 20, hdg);
    _mav_put_uint8_t(buf, 22, peer_id);
    _mav_put_uint8_t(buf, 23, static_pos);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_PEER_POSITION_LEN);
#else
    mavlink_peer_position_t packet;
    packet.time_boot_ms = time_boot_ms;
    packet.lat = lat;
    packet.lon = lon;
    packet.alt = alt;
    packet.relative_alt = relative_alt;
    packet.hdg = hdg;
    packet.peer_id = peer_id;
    packet.static_pos = static_pos;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_PEER_POSITION_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_PEER_POSITION;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_PEER_POSITION_MIN_LEN, MAVLINK_MSG_ID_PEER_POSITION_LEN, MAVLINK_MSG_ID_PEER_POSITION_CRC);
}

/**
 * @brief Pack a peer_position message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_boot_ms [ms] Timestamp (time since system boot).
 * @param peer_id  system's id
 * @param static_pos  vehicle positon is static or dynamic
 * @param lat [degE7] Latitude, expressed
 * @param lon [degE7] Longitude, expressed
 * @param alt [mm] Altitude (MSL). Note that virtually all GPS modules provide both WGS84 and MSL.
 * @param relative_alt [mm] Altitude above ground
 * @param hdg [cdeg] Vehicle heading (yaw angle), 0.0..359.99 degrees. If unknown, set to: UINT16_MAX
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_peer_position_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint32_t time_boot_ms,uint8_t peer_id,uint8_t static_pos,int32_t lat,int32_t lon,int32_t alt,int32_t relative_alt,uint16_t hdg)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_PEER_POSITION_LEN];
    _mav_put_uint32_t(buf, 0, time_boot_ms);
    _mav_put_int32_t(buf, 4, lat);
    _mav_put_int32_t(buf, 8, lon);
    _mav_put_int32_t(buf, 12, alt);
    _mav_put_int32_t(buf, 16, relative_alt);
    _mav_put_uint16_t(buf, 20, hdg);
    _mav_put_uint8_t(buf, 22, peer_id);
    _mav_put_uint8_t(buf, 23, static_pos);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_PEER_POSITION_LEN);
#else
    mavlink_peer_position_t packet;
    packet.time_boot_ms = time_boot_ms;
    packet.lat = lat;
    packet.lon = lon;
    packet.alt = alt;
    packet.relative_alt = relative_alt;
    packet.hdg = hdg;
    packet.peer_id = peer_id;
    packet.static_pos = static_pos;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_PEER_POSITION_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_PEER_POSITION;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_PEER_POSITION_MIN_LEN, MAVLINK_MSG_ID_PEER_POSITION_LEN, MAVLINK_MSG_ID_PEER_POSITION_CRC);
}

/**
 * @brief Encode a peer_position struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param peer_position C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_peer_position_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_peer_position_t* peer_position)
{
    return mavlink_msg_peer_position_pack(system_id, component_id, msg, peer_position->time_boot_ms, peer_position->peer_id, peer_position->static_pos, peer_position->lat, peer_position->lon, peer_position->alt, peer_position->relative_alt, peer_position->hdg);
}

/**
 * @brief Encode a peer_position struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param peer_position C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_peer_position_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_peer_position_t* peer_position)
{
    return mavlink_msg_peer_position_pack_chan(system_id, component_id, chan, msg, peer_position->time_boot_ms, peer_position->peer_id, peer_position->static_pos, peer_position->lat, peer_position->lon, peer_position->alt, peer_position->relative_alt, peer_position->hdg);
}

/**
 * @brief Send a peer_position message
 * @param chan MAVLink channel to send the message
 *
 * @param time_boot_ms [ms] Timestamp (time since system boot).
 * @param peer_id  system's id
 * @param static_pos  vehicle positon is static or dynamic
 * @param lat [degE7] Latitude, expressed
 * @param lon [degE7] Longitude, expressed
 * @param alt [mm] Altitude (MSL). Note that virtually all GPS modules provide both WGS84 and MSL.
 * @param relative_alt [mm] Altitude above ground
 * @param hdg [cdeg] Vehicle heading (yaw angle), 0.0..359.99 degrees. If unknown, set to: UINT16_MAX
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_peer_position_send(mavlink_channel_t chan, uint32_t time_boot_ms, uint8_t peer_id, uint8_t static_pos, int32_t lat, int32_t lon, int32_t alt, int32_t relative_alt, uint16_t hdg)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_PEER_POSITION_LEN];
    _mav_put_uint32_t(buf, 0, time_boot_ms);
    _mav_put_int32_t(buf, 4, lat);
    _mav_put_int32_t(buf, 8, lon);
    _mav_put_int32_t(buf, 12, alt);
    _mav_put_int32_t(buf, 16, relative_alt);
    _mav_put_uint16_t(buf, 20, hdg);
    _mav_put_uint8_t(buf, 22, peer_id);
    _mav_put_uint8_t(buf, 23, static_pos);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_PEER_POSITION, buf, MAVLINK_MSG_ID_PEER_POSITION_MIN_LEN, MAVLINK_MSG_ID_PEER_POSITION_LEN, MAVLINK_MSG_ID_PEER_POSITION_CRC);
#else
    mavlink_peer_position_t packet;
    packet.time_boot_ms = time_boot_ms;
    packet.lat = lat;
    packet.lon = lon;
    packet.alt = alt;
    packet.relative_alt = relative_alt;
    packet.hdg = hdg;
    packet.peer_id = peer_id;
    packet.static_pos = static_pos;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_PEER_POSITION, (const char *)&packet, MAVLINK_MSG_ID_PEER_POSITION_MIN_LEN, MAVLINK_MSG_ID_PEER_POSITION_LEN, MAVLINK_MSG_ID_PEER_POSITION_CRC);
#endif
}

/**
 * @brief Send a peer_position message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_peer_position_send_struct(mavlink_channel_t chan, const mavlink_peer_position_t* peer_position)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_peer_position_send(chan, peer_position->time_boot_ms, peer_position->peer_id, peer_position->static_pos, peer_position->lat, peer_position->lon, peer_position->alt, peer_position->relative_alt, peer_position->hdg);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_PEER_POSITION, (const char *)peer_position, MAVLINK_MSG_ID_PEER_POSITION_MIN_LEN, MAVLINK_MSG_ID_PEER_POSITION_LEN, MAVLINK_MSG_ID_PEER_POSITION_CRC);
#endif
}

#if MAVLINK_MSG_ID_PEER_POSITION_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_peer_position_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t time_boot_ms, uint8_t peer_id, uint8_t static_pos, int32_t lat, int32_t lon, int32_t alt, int32_t relative_alt, uint16_t hdg)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint32_t(buf, 0, time_boot_ms);
    _mav_put_int32_t(buf, 4, lat);
    _mav_put_int32_t(buf, 8, lon);
    _mav_put_int32_t(buf, 12, alt);
    _mav_put_int32_t(buf, 16, relative_alt);
    _mav_put_uint16_t(buf, 20, hdg);
    _mav_put_uint8_t(buf, 22, peer_id);
    _mav_put_uint8_t(buf, 23, static_pos);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_PEER_POSITION, buf, MAVLINK_MSG_ID_PEER_POSITION_MIN_LEN, MAVLINK_MSG_ID_PEER_POSITION_LEN, MAVLINK_MSG_ID_PEER_POSITION_CRC);
#else
    mavlink_peer_position_t *packet = (mavlink_peer_position_t *)msgbuf;
    packet->time_boot_ms = time_boot_ms;
    packet->lat = lat;
    packet->lon = lon;
    packet->alt = alt;
    packet->relative_alt = relative_alt;
    packet->hdg = hdg;
    packet->peer_id = peer_id;
    packet->static_pos = static_pos;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_PEER_POSITION, (const char *)packet, MAVLINK_MSG_ID_PEER_POSITION_MIN_LEN, MAVLINK_MSG_ID_PEER_POSITION_LEN, MAVLINK_MSG_ID_PEER_POSITION_CRC);
#endif
}
#endif

#endif

// MESSAGE PEER_POSITION UNPACKING


/**
 * @brief Get field time_boot_ms from peer_position message
 *
 * @return [ms] Timestamp (time since system boot).
 */
static inline uint32_t mavlink_msg_peer_position_get_time_boot_ms(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field peer_id from peer_position message
 *
 * @return  system's id
 */
static inline uint8_t mavlink_msg_peer_position_get_peer_id(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  22);
}

/**
 * @brief Get field static_pos from peer_position message
 *
 * @return  vehicle positon is static or dynamic
 */
static inline uint8_t mavlink_msg_peer_position_get_static_pos(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  23);
}

/**
 * @brief Get field lat from peer_position message
 *
 * @return [degE7] Latitude, expressed
 */
static inline int32_t mavlink_msg_peer_position_get_lat(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int32_t(msg,  4);
}

/**
 * @brief Get field lon from peer_position message
 *
 * @return [degE7] Longitude, expressed
 */
static inline int32_t mavlink_msg_peer_position_get_lon(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int32_t(msg,  8);
}

/**
 * @brief Get field alt from peer_position message
 *
 * @return [mm] Altitude (MSL). Note that virtually all GPS modules provide both WGS84 and MSL.
 */
static inline int32_t mavlink_msg_peer_position_get_alt(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int32_t(msg,  12);
}

/**
 * @brief Get field relative_alt from peer_position message
 *
 * @return [mm] Altitude above ground
 */
static inline int32_t mavlink_msg_peer_position_get_relative_alt(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int32_t(msg,  16);
}

/**
 * @brief Get field hdg from peer_position message
 *
 * @return [cdeg] Vehicle heading (yaw angle), 0.0..359.99 degrees. If unknown, set to: UINT16_MAX
 */
static inline uint16_t mavlink_msg_peer_position_get_hdg(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint16_t(msg,  20);
}

/**
 * @brief Decode a peer_position message into a struct
 *
 * @param msg The message to decode
 * @param peer_position C-struct to decode the message contents into
 */
static inline void mavlink_msg_peer_position_decode(const mavlink_message_t* msg, mavlink_peer_position_t* peer_position)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    peer_position->time_boot_ms = mavlink_msg_peer_position_get_time_boot_ms(msg);
    peer_position->lat = mavlink_msg_peer_position_get_lat(msg);
    peer_position->lon = mavlink_msg_peer_position_get_lon(msg);
    peer_position->alt = mavlink_msg_peer_position_get_alt(msg);
    peer_position->relative_alt = mavlink_msg_peer_position_get_relative_alt(msg);
    peer_position->hdg = mavlink_msg_peer_position_get_hdg(msg);
    peer_position->peer_id = mavlink_msg_peer_position_get_peer_id(msg);
    peer_position->static_pos = mavlink_msg_peer_position_get_static_pos(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_PEER_POSITION_LEN? msg->len : MAVLINK_MSG_ID_PEER_POSITION_LEN;
        memset(peer_position, 0, MAVLINK_MSG_ID_PEER_POSITION_LEN);
    memcpy(peer_position, _MAV_PAYLOAD(msg), len);
#endif
}
