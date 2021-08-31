package main

import (
	"bytes"
	"encoding/binary"
	"log"
	"fmt"
)


type CorePacket struct {
	name string
	Parse func(*bytes.Reader) map[string]interface{}
}


var corePacketTypeMap = map[uint16]CorePacket {
	0x0001: CorePacket {
		name: "SessionRequest",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			clientCRCLength := ReadUint32be(reader)
			clientSessionID := ReadUint32be(reader)
			clientUDPLength := ReadUint32be(reader)
			protocol := ReadNullTerminatedString(reader)

			result := map[string]interface{} {
				"clientCRCLength": clientCRCLength,
				"clientSessionID": clientSessionID,
				"clientUDPLength": clientUDPLength,
				"protocol": protocol,
			}
			return result
		},
	},
	0x0002: CorePacket {
		name: "SessionReply",
		Parse: func(reader *bytes.Reader) map[string]interface{} {
			serverSessionID := ReadUint32be(reader)
			serverCRCSeed := ReadUint32be(reader)
			serverCRCLength := ReadUint8(reader)
			serverCompression := ReadUint16be(reader)
			serverUDPLength := ReadUint32be(reader)

			result := map[string]interface{} {
				"serverSessionID": serverSessionID,
				"serverCRCSeed": serverCRCSeed,
				"serverCRCLength": serverCRCLength,
				"serverCompression": serverCompression,
				"serverUDPLength": serverUDPLength,
			}
			return result
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

func ReadUint8(reader *bytes.Reader) uint8 {
	result, _ := reader.ReadByte()
	return result
}

func ReadUint16be(reader *bytes.Reader) (result uint16) {
	binary.Read(reader, binary.BigEndian, &result)
	return result
}

func ReadUint32be(reader *bytes.Reader) (result uint32) {
	binary.Read(reader, binary.BigEndian, &result)
	return result
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

func IdentifyPacketName(reader *bytes.Reader) string {
	opcode := ReadUint16be(reader)
	for k, v := range corePacketTypeMap {
		if opcode == k {
			// TODO(rhett): 
			result := v.Parse(reader)
			if result != nil {
				fmt.Println()
				log.Printf("=== %s ========================\n", v.name)
				for k, v := range result {
					log.Printf("- %-24s: %v\n", k, v)
				}
				fmt.Println()
			}
			return v.name
		}
	}
	return "Unknown"
}
