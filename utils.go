package main

import (
	"bytes"
	"encoding/binary"
	"log"
)


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
