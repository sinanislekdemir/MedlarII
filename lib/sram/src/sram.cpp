#include "sram.h"

#include <Arduino.h>
#include <stdint.h>

SRam::SRam() {}

SRam::~SRam() { this->close(); }

void SRam::open(const char *filename) {
  if (SD.exists(filename)) {
    SD.remove(filename);
  }
  // FILE_WRITE != O_RDWR
  this->ram = SD.open(filename, FILE_WRITE);
  this->ram.close();
  this->ram = SD.open(filename, O_RDWR);
  if (!this->ram) {
    return;
  }
  this->isOpen = true;
}

void SRam::close() {
  if (this->isOpen) {
    this->ram.flush();
    this->ram.close();
  }
}

memoryBlockHeader SRam::findVariable(char *name, uint16_t pid) {
  // Idea!
  // PID | VARNAME | USED | POSITION | SIZE | VARIABLE-DATA | PID | VARNAME |
  // USED... Header followed by the value. then the next variable header.
  memoryBlockHeader variable;
  uint32_t pos = 0;
  variable.exists = 0;
  if (!this->isOpen) {
    return variable;
  }
  this->ram.seek(0);
  while (this->ram.readBytes((char *)(&variable), HeaderSize) > 0) {
    if (!variable.exists || variable.pid != pid ||
        strcmp(variable.varname, name) != 0) {
      pos += HeaderSize +
             variable.size; // fast forward to the next variable position
      bool seek = this->ram.seek(pos);
      if (!seek) {
        break;
      }
      continue;
    }
    variable.position = pos + HeaderSize;
    return variable;
  }
  return variable;
}

void SRam::allocateVariable(char *name, uint16_t pid, uint16_t variableSize) {
  memoryBlockHeader variable;
  if (!this->isOpen) {
    return;
  }
  this->ram.seek(this->ram.size());
  strcpy(variable.varname, name);
  variable.exists = 1;
  variable.size = variableSize;
  variable.pid = pid;
  this->ram.write((char *)(&variable), HeaderSize);
  // allocate extra space for the variable
  for (uint16_t i = 0; i < variableSize; i++) {
    this->ram.write("\0", 1);
    if (i % 512 == 0) {
      this->ram.flush();
    }
  }
}

void SRam::deleteVariable(char *name, uint16_t pid) {
  memoryBlockHeader variable = this->findVariable(name, pid);
  if (variable.exists == 0) {
    return;
  }

  uint32_t location = variable.position - HeaderSize +1;
  variable.exists = 0;

  this->ram.seek(location);
  this->ram.write((char *)(&variable), HeaderSize);
  this->ram.flush();
}

uint16_t SRam::read(char *name, uint16_t pid, uint32_t pos, char *buffer,
                    uint16_t size) {
  memoryBlockHeader variable = this->findVariable(name, pid);
  if (variable.exists == 0) {
    return 0;
  }
  if (pos > variable.size) {
    return 0;
  }
  if (pos + size > variable.size) {
    size = variable.size - pos;
  }
  bool seek_check = this->ram.seek(variable.position);
  if (!seek_check) {
    return 0;
  }
  return this->ram.readBytes(buffer, size);
}

uint16_t SRam::readAll(char *name, uint16_t pid, char *buffer) {
  memoryBlockHeader variable = this->findVariable(name, pid);
  return this->read(name, pid, 0, buffer, variable.size);
}

uint16_t SRam::write(char *name, uint16_t pid, uint32_t pos, char *data,
                     uint16_t size) {
  memoryBlockHeader variable = this->findVariable(name, pid);
  if (variable.exists == 0) {
    return 0;
  }
  if (size + pos > variable.size) {
    return 0;
  }
  bool seek_check = this->ram.seek(variable.position + pos);
  if (!seek_check) {
    return 0;
  }
  uint16_t size_w = this->ram.write(data, size);
  this->ram.flush();
  return size_w;
}
