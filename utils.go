package main

import (
	"bytes"
	"encoding/binary"
	"log"
)


func ReadUint8(byteBuffer *bytes.Buffer) uint8 {
	result, _ := byteBuffer.ReadByte()
	return result
}

func ReadUint16be(byteBuffer *bytes.Buffer) (result uint16) {
	binary.Read(byteBuffer, binary.BigEndian, &result)
	return result
}

func ReadUint32be(byteBuffer *bytes.Buffer) (result uint32) {
	binary.Read(byteBuffer, binary.BigEndian, &result)
	return result
}

func ReadNullTerminatedString(byteBuffer *bytes.Buffer) string {
	buffer := make([]byte, 255)
	var string_length int
	for i, _ := range buffer {
		temp_char, err := byteBuffer.ReadByte()
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
