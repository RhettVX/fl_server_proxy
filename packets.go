package main

import (
	"bytes"
	"encoding/binary"
	"log"
)


type CorePacket struct {
	name string
	Parse func(*bytes.Reader) map[string]interface{}
}


var core_packet_type_map = map[uint16]CorePacket {
	0x0001: CorePacket {
		name: "SessionRequest",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			var clientCRCLength uint32
			var clientSessionId uint32
			var clientUDPLength uint32
			var protocol		string

			binary.Read(reader, binary.BigEndian, &clientCRCLength)
			binary.Read(reader, binary.BigEndian, &clientSessionId)
			binary.Read(reader, binary.BigEndian, &clientUDPLength)
			protocol = ReadNullTerminatedString(reader)

			result := map[string]interface{} {
				"clientCRCLength": clientCRCLength,
				"clientSessionId": clientSessionId,
				"clientUDPLength": clientUDPLength,
				"protocol": protocol,
			}
			return result
		},
	},
	0x0002: CorePacket {
		name: "SessionReply",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0003: CorePacket {
		name: "MultiPacket",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0005: CorePacket {
		name: "Disconnect",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0006: CorePacket {
		name: "Ping",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0007: CorePacket {
		name: "NetStatusRequest",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0008: CorePacket {
		name: "NetStatusReply",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0009: CorePacket {
		name: "Data",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x000d: CorePacket {
		name: "DataFragment",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0011: CorePacket {
		name: "OutOfOrder",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0015: CorePacket {
		name: "Ack",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x001d: CorePacket {
		name: "FatalError",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x001e: CorePacket {
		name: "FatalErrorReply",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
}

func ReadNullTerminatedString(reader *bytes.Reader) string {
	buffer := make([]byte, 255)
	var string_length int
	for i, _ := range buffer {
		temp_char, err := reader.ReadByte()
		if err != nil {
			log.Println("Error:", err.Error())
			return ""
		}
		if temp_char == 0 {
			string_length = i + 1
			break
		}
		buffer[i] = temp_char
	}
	return string(buffer[:string_length])
}

func IdentifyPacket(reader *bytes.Reader) string {
	var opcode uint16
	binary.Read(reader, binary.BigEndian, &opcode)
	for k, v := range core_packet_type_map {
		if opcode == k {
			test := v.Parse(reader)
			if opcode == 1 {
				log.Println(test["clientCRCLength"])
				log.Println(test["clientSessionId"])
				log.Println(test["clientUDPLength"])
				log.Println(test["protocol"])
			}
			return v.name
		}
	}
	return "Unknown"
}
