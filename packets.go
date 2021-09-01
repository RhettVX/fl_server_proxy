package main

import (
	"bytes"
	"log"
	"fmt"
	"os"
	"net"
	"encoding/hex"
	"strconv"
)


type CorePacket struct {
	name string
	Parse func(*bytes.Reader, int, int, bool, interface{}) map[string]interface{}
}


var corePacketTypeMap = map[uint16]CorePacket {
	0x0001: CorePacket {
		name: "SessionRequest",
		Parse: func(reader *bytes.Reader, crcSeed, compression int, isSubPacket bool, appData interface{}) map[string]interface{} {
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
		Parse: func(reader *bytes.Reader, crcSeed, compression int, isSubPacket bool, appData interface{}) map[string]interface{} {
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
		Parse: func(reader *bytes.Reader, crcSeed, compression int, isSubPacket bool, appData interface{}) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0005: CorePacket {
		name: "Disconnect",
		Parse: func(reader *bytes.Reader, crcSeed, compression int, isSubPacket bool, appData interface{}) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0006: CorePacket {
		name: "Ping",
		Parse: func(reader *bytes.Reader, crcSeed, compression int, isSubPacket bool, appData interface{}) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0007: CorePacket {
		name: "NetStatusRequest",
		Parse: func(reader *bytes.Reader, crcSeed, compression int, isSubPacket bool, appData interface{}) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0008: CorePacket {
		name: "NetStatusReply",
		Parse: func(reader *bytes.Reader, crcSeed, compression int, isSubPacket bool, appData interface{}) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0009: CorePacket {
		name: "Data",
		Parse: func(reader *bytes.Reader, crcSeed, compression int, isSubPacket bool, appData interface{}) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x000d: CorePacket {
		name: "DataFragment",
		Parse: func(reader *bytes.Reader, crcSeed, compression int, isSubPacket bool, appData interface{}) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0011: CorePacket {
		name: "OutOfOrder",
		Parse: func(reader *bytes.Reader, crcSeed, compression int, isSubPacket bool, appData interface{}) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x0015: CorePacket {
		name: "Ack",
		Parse: func(reader *bytes.Reader, crcSeed, compression int, isSubPacket bool, appData interface{}) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x001d: CorePacket {
		name: "FatalError",
		Parse: func(reader *bytes.Reader, crcSeed, compression int, isSubPacket bool, appData interface{}) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
	0x001e: CorePacket {
		name: "FatalErrorReply",
		Parse: func(reader *bytes.Reader, crcSeed, compression int, isSubPacket bool, appData interface{}) map[string]interface{} {
			log.Println("[!] Not implemented.")
			var empty map[string]interface{}
			return empty
		},
	},
}

// TODO(rhett): We will remove some parameters as they become unnecessary
func HandlePacket(data []byte, dataLength int, remoteAddress *net.UDPAddr,  packetCount int, isFromClient bool) {
	var temp_str string
	if isFromClient {
		temp_str = "c"
	} else {
		temp_str = "s"
	}
	f, err := os.Create(temp_str + strconv.Itoa(packetCount) + "_" + IdentifyPacketName(data[:dataLength]) + ".bin")
	if err != nil {
		log.Fatalln("[X] Unable to create file:", err.Error())
	}
	_, err = f.Write(data[0:dataLength])
	if err != nil {
		log.Fatalln("[X] Unable to write to file:", err.Error())
	}
	err = f.Close()
	if err != nil {
		log.Fatalln("[X] Unable to close file:", err.Error())
	}
	packetCount += 1
	log.Printf("[*] (%s) Recieved %d bytes: \n%s\n", remoteAddress, dataLength, hex.Dump(data[:dataLength]))

	InterceptPacket(data[:dataLength])

}

func InterceptPacket(data []byte) {
	reader := bytes.NewReader(data[:])
	opcode := ReadUint16be(reader)
	for k, v := range corePacketTypeMap {
		if opcode == k {
			// TODO(rhett): 
			result := v.Parse(reader, 0, 0, false, nil)
			if result != nil {
				fmt.Println()
				log.Printf("=== [ %#x ] %s ========================\n", opcode, v.name)
				for k, v := range result {
					log.Printf("- %-24s: %v\n", k, v)
				}
				fmt.Println()
			}
		}
	}
}

func IdentifyPacketName(data []byte) string {
	reader := bytes.NewReader(data[:])
	opcode := ReadUint16be(reader)
	for k, v := range corePacketTypeMap {
		if opcode == k {
			return v.name
		}
	}
	return "Unknown"
}
