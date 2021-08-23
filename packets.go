package main

import (
	"bytes"
	"encoding/binary"
)

var packet_type_map = map[uint16]string {
	0x0001: "SessionRequest",
	0x0002: "SessionReply",
	0x0003: "MultiPacket",
	0x0005: "Disconnect",
	0x0006: "Ping",
	0x0007: "NetStatusRequest",
	0x0008: "NetStatusReply",
	0x0009: "Data",
	0x000d: "DataFragment",
	0x0011: "OutOfOrder",
	0x0015: "Ack",
	0x001d: "FatalError",
	0x001e: "FatalErrorReply",
}

func IdentifyPacket(reader *bytes.Reader) string {
	var opcode uint16
	binary.Read(reader, binary.BigEndian, &opcode)
	for k, v := range packet_type_map {
		if opcode == k {
			return v
		}
	}
	return "Unknown"
}
