/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 */
#include "VaeRubyd.h"



uint32_t VaeRubyd_ping_args::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_ping_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("VaeRubyd_ping_args");
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_ping_pargs::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("VaeRubyd_ping_pargs");
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_ping_result::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 0:
        if (ftype == ::apache::thrift::protocol::T_BYTE) {
          xfer += iprot->readByte(this->success);
          this->__isset.success = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_ping_result::write(::apache::thrift::protocol::TProtocol* oprot) const {

  uint32_t xfer = 0;

  xfer += oprot->writeStructBegin("VaeRubyd_ping_result");

  if (this->__isset.success) {
    xfer += oprot->writeFieldBegin("success", ::apache::thrift::protocol::T_BYTE, 0);
    xfer += oprot->writeByte(this->success);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_ping_presult::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 0:
        if (ftype == ::apache::thrift::protocol::T_BYTE) {
          xfer += iprot->readByte((*(this->success)));
          this->__isset.success = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_fixDocRoot_args::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->path);
          this->__isset.path = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_fixDocRoot_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("VaeRubyd_fixDocRoot_args");
  xfer += oprot->writeFieldBegin("path", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->path);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_fixDocRoot_pargs::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("VaeRubyd_fixDocRoot_pargs");
  xfer += oprot->writeFieldBegin("path", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString((*(this->path)));
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_fixDocRoot_result::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 0:
        if (ftype == ::apache::thrift::protocol::T_BYTE) {
          xfer += iprot->readByte(this->success);
          this->__isset.success = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_fixDocRoot_result::write(::apache::thrift::protocol::TProtocol* oprot) const {

  uint32_t xfer = 0;

  xfer += oprot->writeStructBegin("VaeRubyd_fixDocRoot_result");

  if (this->__isset.success) {
    xfer += oprot->writeFieldBegin("success", ::apache::thrift::protocol::T_BYTE, 0);
    xfer += oprot->writeByte(this->success);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_fixDocRoot_presult::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 0:
        if (ftype == ::apache::thrift::protocol::T_BYTE) {
          xfer += iprot->readByte((*(this->success)));
          this->__isset.success = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_haml_args::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->text);
          this->__isset.text = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_haml_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("VaeRubyd_haml_args");
  xfer += oprot->writeFieldBegin("text", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->text);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_haml_pargs::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("VaeRubyd_haml_pargs");
  xfer += oprot->writeFieldBegin("text", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString((*(this->text)));
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_haml_result::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 0:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->success);
          this->__isset.success = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->se.read(iprot);
          this->__isset.se = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_haml_result::write(::apache::thrift::protocol::TProtocol* oprot) const {

  uint32_t xfer = 0;

  xfer += oprot->writeStructBegin("VaeRubyd_haml_result");

  if (this->__isset.success) {
    xfer += oprot->writeFieldBegin("success", ::apache::thrift::protocol::T_STRING, 0);
    xfer += oprot->writeString(this->success);
    xfer += oprot->writeFieldEnd();
  } else if (this->__isset.se) {
    xfer += oprot->writeFieldBegin("se", ::apache::thrift::protocol::T_STRUCT, 1);
    xfer += this->se.write(oprot);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_haml_presult::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 0:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString((*(this->success)));
          this->__isset.success = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->se.read(iprot);
          this->__isset.se = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_sass_args::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->text);
          this->__isset.text = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->load_path);
          this->__isset.load_path = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_sass_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("VaeRubyd_sass_args");
  xfer += oprot->writeFieldBegin("text", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->text);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("load_path", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString(this->load_path);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_sass_pargs::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("VaeRubyd_sass_pargs");
  xfer += oprot->writeFieldBegin("text", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString((*(this->text)));
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("load_path", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString((*(this->load_path)));
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_sass_result::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 0:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->success);
          this->__isset.success = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->se.read(iprot);
          this->__isset.se = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_sass_result::write(::apache::thrift::protocol::TProtocol* oprot) const {

  uint32_t xfer = 0;

  xfer += oprot->writeStructBegin("VaeRubyd_sass_result");

  if (this->__isset.success) {
    xfer += oprot->writeFieldBegin("success", ::apache::thrift::protocol::T_STRING, 0);
    xfer += oprot->writeString(this->success);
    xfer += oprot->writeFieldEnd();
  } else if (this->__isset.se) {
    xfer += oprot->writeFieldBegin("se", ::apache::thrift::protocol::T_STRUCT, 1);
    xfer += this->se.write(oprot);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_sass_presult::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 0:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString((*(this->success)));
          this->__isset.success = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->se.read(iprot);
          this->__isset.se = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_scss_args::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->text);
          this->__isset.text = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->load_path);
          this->__isset.load_path = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_scss_args::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("VaeRubyd_scss_args");
  xfer += oprot->writeFieldBegin("text", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString(this->text);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("load_path", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString(this->load_path);
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_scss_pargs::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin("VaeRubyd_scss_pargs");
  xfer += oprot->writeFieldBegin("text", ::apache::thrift::protocol::T_STRING, 1);
  xfer += oprot->writeString((*(this->text)));
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldBegin("load_path", ::apache::thrift::protocol::T_STRING, 2);
  xfer += oprot->writeString((*(this->load_path)));
  xfer += oprot->writeFieldEnd();
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_scss_result::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 0:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString(this->success);
          this->__isset.success = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->se.read(iprot);
          this->__isset.se = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t VaeRubyd_scss_result::write(::apache::thrift::protocol::TProtocol* oprot) const {

  uint32_t xfer = 0;

  xfer += oprot->writeStructBegin("VaeRubyd_scss_result");

  if (this->__isset.success) {
    xfer += oprot->writeFieldBegin("success", ::apache::thrift::protocol::T_STRING, 0);
    xfer += oprot->writeString(this->success);
    xfer += oprot->writeFieldEnd();
  } else if (this->__isset.se) {
    xfer += oprot->writeFieldBegin("se", ::apache::thrift::protocol::T_STRUCT, 1);
    xfer += this->se.write(oprot);
    xfer += oprot->writeFieldEnd();
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t VaeRubyd_scss_presult::read(::apache::thrift::protocol::TProtocol* iprot) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 0:
        if (ftype == ::apache::thrift::protocol::T_STRING) {
          xfer += iprot->readString((*(this->success)));
          this->__isset.success = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 1:
        if (ftype == ::apache::thrift::protocol::T_STRUCT) {
          xfer += this->se.read(iprot);
          this->__isset.se = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

int8_t VaeRubydClient::ping()
{
  send_ping();
  return recv_ping();
}

void VaeRubydClient::send_ping()
{
  int32_t cseqid = 0;
  oprot_->writeMessageBegin("ping", ::apache::thrift::protocol::T_CALL, cseqid);

  VaeRubyd_ping_pargs args;
  args.write(oprot_);

  oprot_->writeMessageEnd();
  oprot_->getTransport()->flush();
  oprot_->getTransport()->writeEnd();
}

int8_t VaeRubydClient::recv_ping()
{

  int32_t rseqid = 0;
  std::string fname;
  ::apache::thrift::protocol::TMessageType mtype;

  iprot_->readMessageBegin(fname, mtype, rseqid);
  if (mtype == ::apache::thrift::protocol::T_EXCEPTION) {
    ::apache::thrift::TApplicationException x;
    x.read(iprot_);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw x;
  }
  if (mtype != ::apache::thrift::protocol::T_REPLY) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::INVALID_MESSAGE_TYPE);
  }
  if (fname.compare("ping") != 0) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::WRONG_METHOD_NAME);
  }
  int8_t _return;
  VaeRubyd_ping_presult result;
  result.success = &_return;
  result.read(iprot_);
  iprot_->readMessageEnd();
  iprot_->getTransport()->readEnd();

  if (result.__isset.success) {
    return _return;
  }
  throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::MISSING_RESULT, "ping failed: unknown result");
}

int8_t VaeRubydClient::fixDocRoot(const std::string& path)
{
  send_fixDocRoot(path);
  return recv_fixDocRoot();
}

void VaeRubydClient::send_fixDocRoot(const std::string& path)
{
  int32_t cseqid = 0;
  oprot_->writeMessageBegin("fixDocRoot", ::apache::thrift::protocol::T_CALL, cseqid);

  VaeRubyd_fixDocRoot_pargs args;
  args.path = &path;
  args.write(oprot_);

  oprot_->writeMessageEnd();
  oprot_->getTransport()->flush();
  oprot_->getTransport()->writeEnd();
}

int8_t VaeRubydClient::recv_fixDocRoot()
{

  int32_t rseqid = 0;
  std::string fname;
  ::apache::thrift::protocol::TMessageType mtype;

  iprot_->readMessageBegin(fname, mtype, rseqid);
  if (mtype == ::apache::thrift::protocol::T_EXCEPTION) {
    ::apache::thrift::TApplicationException x;
    x.read(iprot_);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw x;
  }
  if (mtype != ::apache::thrift::protocol::T_REPLY) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::INVALID_MESSAGE_TYPE);
  }
  if (fname.compare("fixDocRoot") != 0) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::WRONG_METHOD_NAME);
  }
  int8_t _return;
  VaeRubyd_fixDocRoot_presult result;
  result.success = &_return;
  result.read(iprot_);
  iprot_->readMessageEnd();
  iprot_->getTransport()->readEnd();

  if (result.__isset.success) {
    return _return;
  }
  throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::MISSING_RESULT, "fixDocRoot failed: unknown result");
}

void VaeRubydClient::haml(std::string& _return, const std::string& text)
{
  send_haml(text);
  recv_haml(_return);
}

void VaeRubydClient::send_haml(const std::string& text)
{
  int32_t cseqid = 0;
  oprot_->writeMessageBegin("haml", ::apache::thrift::protocol::T_CALL, cseqid);

  VaeRubyd_haml_pargs args;
  args.text = &text;
  args.write(oprot_);

  oprot_->writeMessageEnd();
  oprot_->getTransport()->flush();
  oprot_->getTransport()->writeEnd();
}

void VaeRubydClient::recv_haml(std::string& _return)
{

  int32_t rseqid = 0;
  std::string fname;
  ::apache::thrift::protocol::TMessageType mtype;

  iprot_->readMessageBegin(fname, mtype, rseqid);
  if (mtype == ::apache::thrift::protocol::T_EXCEPTION) {
    ::apache::thrift::TApplicationException x;
    x.read(iprot_);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw x;
  }
  if (mtype != ::apache::thrift::protocol::T_REPLY) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::INVALID_MESSAGE_TYPE);
  }
  if (fname.compare("haml") != 0) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::WRONG_METHOD_NAME);
  }
  VaeRubyd_haml_presult result;
  result.success = &_return;
  result.read(iprot_);
  iprot_->readMessageEnd();
  iprot_->getTransport()->readEnd();

  if (result.__isset.success) {
    // _return pointer has now been filled
    return;
  }
  if (result.__isset.se) {
    throw result.se;
  }
  throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::MISSING_RESULT, "haml failed: unknown result");
}

void VaeRubydClient::sass(std::string& _return, const std::string& text, const std::string& load_path)
{
  send_sass(text, load_path);
  recv_sass(_return);
}

void VaeRubydClient::send_sass(const std::string& text, const std::string& load_path)
{
  int32_t cseqid = 0;
  oprot_->writeMessageBegin("sass", ::apache::thrift::protocol::T_CALL, cseqid);

  VaeRubyd_sass_pargs args;
  args.text = &text;
  args.load_path = &load_path;
  args.write(oprot_);

  oprot_->writeMessageEnd();
  oprot_->getTransport()->flush();
  oprot_->getTransport()->writeEnd();
}

void VaeRubydClient::recv_sass(std::string& _return)
{

  int32_t rseqid = 0;
  std::string fname;
  ::apache::thrift::protocol::TMessageType mtype;

  iprot_->readMessageBegin(fname, mtype, rseqid);
  if (mtype == ::apache::thrift::protocol::T_EXCEPTION) {
    ::apache::thrift::TApplicationException x;
    x.read(iprot_);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw x;
  }
  if (mtype != ::apache::thrift::protocol::T_REPLY) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::INVALID_MESSAGE_TYPE);
  }
  if (fname.compare("sass") != 0) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::WRONG_METHOD_NAME);
  }
  VaeRubyd_sass_presult result;
  result.success = &_return;
  result.read(iprot_);
  iprot_->readMessageEnd();
  iprot_->getTransport()->readEnd();

  if (result.__isset.success) {
    // _return pointer has now been filled
    return;
  }
  if (result.__isset.se) {
    throw result.se;
  }
  throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::MISSING_RESULT, "sass failed: unknown result");
}

void VaeRubydClient::scss(std::string& _return, const std::string& text, const std::string& load_path)
{
  send_scss(text, load_path);
  recv_scss(_return);
}

void VaeRubydClient::send_scss(const std::string& text, const std::string& load_path)
{
  int32_t cseqid = 0;
  oprot_->writeMessageBegin("scss", ::apache::thrift::protocol::T_CALL, cseqid);

  VaeRubyd_scss_pargs args;
  args.text = &text;
  args.load_path = &load_path;
  args.write(oprot_);

  oprot_->writeMessageEnd();
  oprot_->getTransport()->flush();
  oprot_->getTransport()->writeEnd();
}

void VaeRubydClient::recv_scss(std::string& _return)
{

  int32_t rseqid = 0;
  std::string fname;
  ::apache::thrift::protocol::TMessageType mtype;

  iprot_->readMessageBegin(fname, mtype, rseqid);
  if (mtype == ::apache::thrift::protocol::T_EXCEPTION) {
    ::apache::thrift::TApplicationException x;
    x.read(iprot_);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw x;
  }
  if (mtype != ::apache::thrift::protocol::T_REPLY) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::INVALID_MESSAGE_TYPE);
  }
  if (fname.compare("scss") != 0) {
    iprot_->skip(::apache::thrift::protocol::T_STRUCT);
    iprot_->readMessageEnd();
    iprot_->getTransport()->readEnd();
    throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::WRONG_METHOD_NAME);
  }
  VaeRubyd_scss_presult result;
  result.success = &_return;
  result.read(iprot_);
  iprot_->readMessageEnd();
  iprot_->getTransport()->readEnd();

  if (result.__isset.success) {
    // _return pointer has now been filled
    return;
  }
  if (result.__isset.se) {
    throw result.se;
  }
  throw ::apache::thrift::TApplicationException(::apache::thrift::TApplicationException::MISSING_RESULT, "scss failed: unknown result");
}

bool VaeRubydProcessor::process(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot) {

  ::apache::thrift::protocol::TProtocol* iprot = piprot.get();
  ::apache::thrift::protocol::TProtocol* oprot = poprot.get();
  std::string fname;
  ::apache::thrift::protocol::TMessageType mtype;
  int32_t seqid;

  iprot->readMessageBegin(fname, mtype, seqid);

  if (mtype != ::apache::thrift::protocol::T_CALL && mtype != ::apache::thrift::protocol::T_ONEWAY) {
    iprot->skip(::apache::thrift::protocol::T_STRUCT);
    iprot->readMessageEnd();
    iprot->getTransport()->readEnd();
    ::apache::thrift::TApplicationException x(::apache::thrift::TApplicationException::INVALID_MESSAGE_TYPE);
    oprot->writeMessageBegin(fname, ::apache::thrift::protocol::T_EXCEPTION, seqid);
    x.write(oprot);
    oprot->writeMessageEnd();
    oprot->getTransport()->flush();
    oprot->getTransport()->writeEnd();
    return true;
  }

  return process_fn(iprot, oprot, fname, seqid);
}

bool VaeRubydProcessor::process_fn(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, std::string& fname, int32_t seqid) {
  std::map<std::string, void (VaeRubydProcessor::*)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*)>::iterator pfn;
  pfn = processMap_.find(fname);
  if (pfn == processMap_.end()) {
    iprot->skip(::apache::thrift::protocol::T_STRUCT);
    iprot->readMessageEnd();
    iprot->getTransport()->readEnd();
    ::apache::thrift::TApplicationException x(::apache::thrift::TApplicationException::UNKNOWN_METHOD, "Invalid method name: '"+fname+"'");
    oprot->writeMessageBegin(fname, ::apache::thrift::protocol::T_EXCEPTION, seqid);
    x.write(oprot);
    oprot->writeMessageEnd();
    oprot->getTransport()->flush();
    oprot->getTransport()->writeEnd();
    return true;
  }
  (this->*(pfn->second))(seqid, iprot, oprot);
  return true;
}

void VaeRubydProcessor::process_ping(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot)
{
  VaeRubyd_ping_args args;
  args.read(iprot);
  iprot->readMessageEnd();
  iprot->getTransport()->readEnd();

  VaeRubyd_ping_result result;
  try {
    result.success = iface_->ping();
    result.__isset.success = true;
  } catch (const std::exception& e) {
    ::apache::thrift::TApplicationException x(e.what());
    oprot->writeMessageBegin("ping", ::apache::thrift::protocol::T_EXCEPTION, seqid);
    x.write(oprot);
    oprot->writeMessageEnd();
    oprot->getTransport()->flush();
    oprot->getTransport()->writeEnd();
    return;
  }

  oprot->writeMessageBegin("ping", ::apache::thrift::protocol::T_REPLY, seqid);
  result.write(oprot);
  oprot->writeMessageEnd();
  oprot->getTransport()->flush();
  oprot->getTransport()->writeEnd();
}

void VaeRubydProcessor::process_fixDocRoot(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot)
{
  VaeRubyd_fixDocRoot_args args;
  args.read(iprot);
  iprot->readMessageEnd();
  iprot->getTransport()->readEnd();

  VaeRubyd_fixDocRoot_result result;
  try {
    result.success = iface_->fixDocRoot(args.path);
    result.__isset.success = true;
  } catch (const std::exception& e) {
    ::apache::thrift::TApplicationException x(e.what());
    oprot->writeMessageBegin("fixDocRoot", ::apache::thrift::protocol::T_EXCEPTION, seqid);
    x.write(oprot);
    oprot->writeMessageEnd();
    oprot->getTransport()->flush();
    oprot->getTransport()->writeEnd();
    return;
  }

  oprot->writeMessageBegin("fixDocRoot", ::apache::thrift::protocol::T_REPLY, seqid);
  result.write(oprot);
  oprot->writeMessageEnd();
  oprot->getTransport()->flush();
  oprot->getTransport()->writeEnd();
}

void VaeRubydProcessor::process_haml(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot)
{
  VaeRubyd_haml_args args;
  args.read(iprot);
  iprot->readMessageEnd();
  iprot->getTransport()->readEnd();

  VaeRubyd_haml_result result;
  try {
    iface_->haml(result.success, args.text);
    result.__isset.success = true;
  } catch (VaeSyntaxError &se) {
    result.se = se;
    result.__isset.se = true;
  } catch (const std::exception& e) {
    ::apache::thrift::TApplicationException x(e.what());
    oprot->writeMessageBegin("haml", ::apache::thrift::protocol::T_EXCEPTION, seqid);
    x.write(oprot);
    oprot->writeMessageEnd();
    oprot->getTransport()->flush();
    oprot->getTransport()->writeEnd();
    return;
  }

  oprot->writeMessageBegin("haml", ::apache::thrift::protocol::T_REPLY, seqid);
  result.write(oprot);
  oprot->writeMessageEnd();
  oprot->getTransport()->flush();
  oprot->getTransport()->writeEnd();
}

void VaeRubydProcessor::process_sass(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot)
{
  VaeRubyd_sass_args args;
  args.read(iprot);
  iprot->readMessageEnd();
  iprot->getTransport()->readEnd();

  VaeRubyd_sass_result result;
  try {
    iface_->sass(result.success, args.text, args.load_path);
    result.__isset.success = true;
  } catch (VaeSyntaxError &se) {
    result.se = se;
    result.__isset.se = true;
  } catch (const std::exception& e) {
    ::apache::thrift::TApplicationException x(e.what());
    oprot->writeMessageBegin("sass", ::apache::thrift::protocol::T_EXCEPTION, seqid);
    x.write(oprot);
    oprot->writeMessageEnd();
    oprot->getTransport()->flush();
    oprot->getTransport()->writeEnd();
    return;
  }

  oprot->writeMessageBegin("sass", ::apache::thrift::protocol::T_REPLY, seqid);
  result.write(oprot);
  oprot->writeMessageEnd();
  oprot->getTransport()->flush();
  oprot->getTransport()->writeEnd();
}

void VaeRubydProcessor::process_scss(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot)
{
  VaeRubyd_scss_args args;
  args.read(iprot);
  iprot->readMessageEnd();
  iprot->getTransport()->readEnd();

  VaeRubyd_scss_result result;
  try {
    iface_->scss(result.success, args.text, args.load_path);
    result.__isset.success = true;
  } catch (VaeSyntaxError &se) {
    result.se = se;
    result.__isset.se = true;
  } catch (const std::exception& e) {
    ::apache::thrift::TApplicationException x(e.what());
    oprot->writeMessageBegin("scss", ::apache::thrift::protocol::T_EXCEPTION, seqid);
    x.write(oprot);
    oprot->writeMessageEnd();
    oprot->getTransport()->flush();
    oprot->getTransport()->writeEnd();
    return;
  }

  oprot->writeMessageBegin("scss", ::apache::thrift::protocol::T_REPLY, seqid);
  result.write(oprot);
  oprot->writeMessageEnd();
  oprot->getTransport()->flush();
  oprot->getTransport()->writeEnd();
}



