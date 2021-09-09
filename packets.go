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
	Parse func(*bytes.Buffer, int, int, bool, []map[string]interface{}) map[string]interface{}
}

var corePacketTypeMap map[uint16]CorePacket


func InitPackets() {
	corePacketTypeMap = map[uint16]CorePacket {
		0x0001: CorePacket {
			name: "SessionRequest",
			Parse: func(byteBuffer *bytes.Buffer, crcSeed, compression int, isSubPacket bool, appData []map[string]interface{}) map[string]interface{} {
				clientCRCLength := ReadUint32be(byteBuffer)
				clientSessionID := ReadUint32be(byteBuffer)
				clientUDPLength := ReadUint32be(byteBuffer)
				protocol := ReadNullTerminatedString(byteBuffer)

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
			Parse: func(byteBuffer *bytes.Buffer, crcSeed, compression int, isSubPacket bool, appData []map[string]interface{}) map[string]interface{} {
				serverSessionID := ReadUint32be(byteBuffer)
				serverCRCSeed := ReadUint32be(byteBuffer)
				serverCRCLength := ReadUint8(byteBuffer)
				serverCompression := ReadUint16be(byteBuffer)
				serverUDPLength := ReadUint32be(byteBuffer)

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
			Parse: func(byteBuffer *bytes.Buffer, crcSeed, compression int, isSubPacket bool, appData []map[string]interface{}) map[string]interface{} {
				// var dataLength uint8 = 0
				// offset := 0

				// if compression != 0 {
				// 	// TODO(rhett): Skip 1 byte
				// 	ReadUint8(byteBuffer)
				// 	offset = 1
				// }

				// for offset < byteBuffer.Len() {
				// 	dataLength = ReadUint8(byteBuffer)
				// 	offset += 1
				// 	// numBytes := 0

				// 	if dataLength == 0xff {
				// 		log.Fatalln("UH OH")
				// 	}
				// 	// numBytes = 1
				// 	ReadUint8(byteBuffer)
				// 	offset += 1

				// 	ParsePacket(byteBuffer.Bytes()[offset:], crcSeed, compression, true, appData)
				// 	break
				// }
				log.Println("[!] Not implemented.")
				var empty map[string]interface{}
				return empty
			},
		},
		0x0005: CorePacket {
			name: "Disconnect",
			Parse: func(byteBuffer *bytes.Buffer, crcSeed, compression int, isSubPacket bool, appData []map[string]interface{}) map[string]interface{} {
				log.Println("[!] Not implemented.")
				var empty map[string]interface{}
				return empty
			},
		},
		0x0006: CorePacket {
			name: "Ping",
			Parse: func(byteBuffer *bytes.Buffer, crcSeed, compression int, isSubPacket bool, appData []map[string]interface{}) map[string]interface{} {
				log.Println("[!] Not implemented.")
				var empty map[string]interface{}
				return empty
			},
		},
		0x0007: CorePacket {
			name: "NetStatusRequest",
			Parse: func(byteBuffer *bytes.Buffer, crcSeed, compression int, isSubPacket bool, appData []map[string]interface{}) map[string]interface{} {
				log.Println("[!] Not implemented.")
				var empty map[string]interface{}
				return empty
			},
		},
		0x0008: CorePacket {
			name: "NetStatusReply",
			Parse: func(byteBuffer *bytes.Buffer, crcSeed, compression int, isSubPacket bool, appData []map[string]interface{}) map[string]interface{} {
				log.Println("[!] Not implemented.")
				var empty map[string]interface{}
				return empty
			},
		},
		0x0009: CorePacket {
			name: "Data",
			Parse: func(byteBuffer *bytes.Buffer, crcSeed, compression int, isSubPacket bool, appData []map[string]interface{}) map[string]interface{} {
				log.Println("[!] Not implemented.")
				var empty map[string]interface{}
				return empty
			},
		},
		0x000d: CorePacket {
			name: "DataFragment",
			Parse: func(byteBuffer *bytes.Buffer, crcSeed, compression int, isSubPacket bool, appData []map[string]interface{}) map[string]interface{} {
				// TODO(rhett): Doing this weirdly for now
				dataSnapshot := byteBuffer.Bytes()
				if compression != 0 && !isSubPacket {
					// TODO(rhett): Skip 1 bytes
					ReadUint8(byteBuffer)
				}

				sequence := ReadUint16be(byteBuffer)
				fragmentEnd := len(dataSnapshot)
				if !isSubPacket {
					fragmentEnd -= 2
				}

				var crc uint16 = 0
				if !isSubPacket {
					crc = ReadUint16be(bytes.NewBuffer(dataSnapshot[fragmentEnd:]))
				}

				dataOffset := 2
				if compression != 0 && !isSubPacket {
					dataOffset += 1
				}

				data := dataSnapshot[dataOffset:fragmentEnd]

				log.Printf("%d\n", sequence)
				log.Printf("%d\n", fragmentEnd)
				log.Printf("%x\n", crc)
				log.Printf("\n%s\n", hex.Dump(data[:]))

				log.Println("[!] Not implemented.")
				var empty map[string]interface{}
				return empty
			},
		},
		0x0011: CorePacket {
			name: "OutOfOrder",
			Parse: func(byteBuffer *bytes.Buffer, crcSeed, compression int, isSubPacket bool, appData []map[string]interface{}) map[string]interface{} {
				log.Println("[!] Not implemented.")
				var empty map[string]interface{}
				return empty
			},
		},
		0x0015: CorePacket {
			name: "Ack",
			Parse: func(byteBuffer *bytes.Buffer, crcSeed, compression int, isSubPacket bool, appData []map[string]interface{}) map[string]interface{} {
				log.Println("[!] Not implemented.")
				var empty map[string]interface{}
				return empty
			},
		},
		0x001d: CorePacket {
			name: "FatalError",
			Parse: func(byteBuffer *bytes.Buffer, crcSeed, compression int, isSubPacket bool, appData []map[string]interface{}) map[string]interface{} {
				log.Println("[!] Not implemented.")
				var empty map[string]interface{}
				return empty
			},
		},
		0x001e: CorePacket {
			name: "FatalErrorReply",
			Parse: func(byteBuffer *bytes.Buffer, crcSeed, compression int, isSubPacket bool, appData []map[string]interface{}) map[string]interface{} {
				log.Println("[!] Not implemented.")
				var empty map[string]interface{}
				return empty
			},
		},
	}
}

// TODO(rhett): We will remove some parameters as they become unnecessary
func HandlePacket(data []byte, dataLength int, remoteAddress *net.UDPAddr,  packetCount int, isFromClient bool) {
	// log.Printf("[*] (%s) Recieved %d bytes: \n%s\n", remoteAddress, dataLength, hex.Dump(data[:dataLength]))
	log.Printf("[*] (%s) Recieved %d bytes\n", remoteAddress, dataLength)
	packetType, packetName, packetResult := ParsePacket(data[:dataLength], 0, 0x0100, false, nil)
	fmt.Println()
	log.Printf("=== [ %#x ] %s ========================\n", packetType, packetName)
	for k, v := range packetResult {
		log.Printf("- %-24s: %v\n", k, v)
	}
	fmt.Println()

	var temp_str string
	if isFromClient {
		temp_str = "c"
	} else {
		temp_str = "s"
	}
	f, err := os.Create(temp_str + strconv.Itoa(packetCount) + "_" + packetName + ".bin")
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
}

func ParsePacket(data        []byte,
				 crcSeed     int,
				 compression int,
				 isSubPacket bool,
				 appData     interface{}) (uint16, string, map[string]interface{}) {
	byteBuffer := bytes.NewBuffer(data[:])

	packetType := ReadUint16be(byteBuffer)
	packet, exists := corePacketTypeMap[packetType]
	if exists {
		result := packet.Parse(byteBuffer, crcSeed, compression, isSubPacket, appData)
		if result != nil {
			return packetType, packet.name, result
		}
		log.Printf("[!] Packet returned no result.")
		return packetType, packet.name, nil
	}
	log.Printf("[!] Unknown packet type: %#x\n", packetType)
	return packetType, "UNKNOWN", nil
}
