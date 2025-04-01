#!/usr/bin/env python3
# vim: set sw=2 expandtab:

import argparse
import base64
import json
import logging
import os
import sys

import ddp.command
import ddp.response
import dep
import wisun.command
import wisun.common
import wisun.response
from ddp.commander import *
from ddp.rtt import SerialWire

################################################## HELPER FUNCTIONS ####################################################################################################

def GetMacAddress(sw):
  logger.info("Retrieving device serial number")
  sn = sw.get_mac_address()
  logger.info("Device serial number: %s", sn)
  return sn

def OpenSWConnection(soc_,jlink_ser,jlink_host, reset = False):
  try:
    soc = wisun.common.socs[soc_]
  except KeyError:
    logger.info(f'{parser.prog}: error: {args.soc} is not a supported SoC type', file=sys.stderr)
    exit(1)

  logger.info("Opening SerialWire connection to the device")
  jlink_xml = os.path.join(os.path.dirname(__file__), "jlink/JLinkDevices.xml")
  sw = SerialWire(soc['device'], jlink_ser,jlink_host, jlink_xml)
  sw.connect()
  if reset:
    sw.reset_and_halt()
    logger.info("Serial Wire connection has been reset.")
  logger.info("Connection opened")
  return (sw, soc)

def CloseSWConnection(sw, reset = False):
  sw.rtt_stop()
  if(reset):    
    sw.reset()
  sw.close()

######################################################################################################################################################

################################################## CALLABLE APIS #####################################################################

def _initNVM(args):
  try:
    sw, soc = OpenSWConnection(args.soc,args.jlink_ser,args.jlink_host, True)
    FlashProvApp(sw,soc,args.prov_img)
    InitNVM(sw,soc)
    sn = GetMacAddress(sw)
    with open('results/mac.txt', 'w') as file:
      file.write(sn)
  except Exception as e:
    logging.exception("_initNVM - Error")
    return exit(1)
  finally:
    CloseSWConnection(sw)
  return exit(0)

def _genKey(args):
  try:
    sw, soc = OpenSWConnection(args.soc,args.jlink_ser,args.jlink_host)
    sw.rtt_start()
    GenerateDeviceKey(sw)
  except Exception as e:
    logging.exception("_genKey - Error")
    return exit(1)
  finally:
    CloseSWConnection(sw)
  return exit(0)

def _genCSR(args):
  try:
    sw , soc = OpenSWConnection(args.soc,args.jlink_ser,args.jlink_host)
    sw.rtt_start()
    csr = GenWisunCSR(sw)
    with open('results/csr.bin', 'wb') as file:
      file.write(csr)
  except Exception as e:
    logging.exception("_genCSR - Error")
    return exit(1)
  finally:
    CloseSWConnection(sw)
  return exit(0)

def _writeDeviceCerts(args):
  try:
    sw, soc = OpenSWConnection(args.soc,args.jlink_ser,args.jlink_host)
    sw.rtt_start()
    WriteWisunCertsToNVM(sw, base64.b64decode(args.data))
  except Exception as e:
    logging.exception("_writeDeviceCerts - Error")
    return exit(1)
  finally:
    CloseSWConnection(sw)
  return exit(0)

def _writeBatchCerts(args):
  try:
    sw, soc = OpenSWConnection(args.soc,args.jlink_ser,args.jlink_host)
    sw.rtt_start()
    WriteBarchToNVM(sw, base64.b64decode(args.data))
  except Exception as e:
    logging.exception("_writeBatchCerts - Error")
    return exit(1)
  finally:
    CloseSWConnection(sw)
  return exit(0)

def _writeRootCerts(args):
  try:
    sw, soc = OpenSWConnection(args.soc,args.jlink_ser,args.jlink_host)
    sw.rtt_start()
    WriteRootNVM(sw, base64.b64decode(args.data))
  except Exception as e:
    logging.exception("_writeRootCerts - Error")
    return exit(1)
  finally:
    CloseSWConnection(sw)
  return exit(0)
  
#########################################################################################################################################


################################################## CORE PROCEDURES #####################################################################

def FlashProvApp(sw, soc, prov_img):
  # Inject and run provisioning application
  with open(prov_img, 'rb') as f:
    provisioning_app = f.read()
  logger.info("Injecting provisioning application")
  ram_addr = soc['ramstartaddress']
  sw.run_application(ram_addr, provisioning_app)
  sw.rtt_start()
  logger.info("Provisioning application running")

def InitNVM(sw,soc):
  # Initialize NVM
  logger.info("Initializing NVM")
  tx = ddp.command.InitializeNvm(soc['nvm3inststartaddress'], soc['nvm3instsize'])
  sw.rtt_send(tx)
  rx = sw.rtt_receive()
  resp = ddp.response.InitializeNvm(rx)
  if resp.status != 0: 
    logger.warning(f"Failure during NVM initialization ({resp.status})")
  logger.info("NVM initialized")


def GenerateDeviceKey(sw:SerialWire):
  logger.info("Generating Wi-SUN key pair on the device")
  tx = wisun.command.GenerateKeyPair(0x100)
  sw.rtt_send(tx)
  rx = sw.rtt_receive()
  resp = wisun.response.GenerateKeyPair(rx)
  assert resp.status in (0,19), f"Failure during Wi-SUN key pair generation ({resp.status})"
  if resp.status == 19:
    logger.warning("Wi-SUN key pair already exists")
  else:
    logger.info("Wi-SUN key pair generated")
    logger.warning("Wi-SUN key pair generated")
  logger.warning(f"Device Key: ({resp.key})")
  return resp.key

def GenWisunCSR(sw):
  # Generate Wi-SUN CSR
  logger.info("Generating Wi-SUN CSR on the device")
  tx = wisun.command.GenerateCsr(0x100)
  sw.rtt_send(tx)
  rx = sw.rtt_receive()
  resp = wisun.response.GenerateCsr(rx)
  assert resp.status == 0, f"Failure during Wi-SUN CSR generation ({resp.status})"
  logger.info("Wi-SUN CSR generated")
  #key = base64.b64encode(resp.csr).decode("utf-8")
  #logger.warning(f"CSR Key: ({key})")
  return resp.csr

def WriteWisunCertsToNVM(sw: SerialWire, device:bytes):
    # Write Wi-SUN device certificate into NVM
    logger.info("Saving Wi-SUN device certificate into NVM")
    tx = ddp.command.WriteNvm(0x100, device)
    sw.rtt_send(tx)
    rx = sw.rtt_receive()
    resp = ddp.response.WriteNvm(rx)
    assert resp.status == 0, f"Failure saving Wi-SUN device certificate into NVM ({resp.status})"
    logger.info("Wi-SUN device certificate saved")

def WriteBarchToNVM(sw: SerialWire, batch:bytes):
    # Write Wi-SUN batch certificate into NVM
    logger.info("Saving Wi-SUN batch certificate into NVM")
    tx = ddp.command.WriteNvm(0x101, batch)
    sw.rtt_send(tx)
    rx = sw.rtt_receive()
    resp = ddp.response.WriteNvm(rx)
    assert resp.status == 0, f"Failure saving Wi-SUN batch certificate into NVM ({resp.status})"
    logger.info("Wi-SUN batch certificate saved")

def WriteRootNVM(sw: SerialWire, root:bytes):
    # Write Wi-SUN root certificate into NVM
    logger.info("Saving Wi-SUN root certificate into NVM")
    tx = ddp.command.WriteNvm(0x102, root)
    sw.rtt_send(tx)
    rx = sw.rtt_receive()
    resp = ddp.response.WriteNvm(rx)
    assert resp.status == 0, f"Failure saving Wi-SUN root certificate into NVM ({resp.status})"
    logger.info("Wi-SUN root certificate saved")

################################################################################################################################################################

if __name__ == '__main__':
  os.makedirs('results', exist_ok=True)
  logging.basicConfig(filename='results/py.log',
                    filemode='w',
                    format='%(asctime)s,%(msecs)03d %(name)s %(levelname)s %(message)s',
                    datefmt='%Y-%m-%d %H:%M:%S',
                    level=logging.INFO)

  logger = logging.getLogger('provision')
  logger.setLevel(logging.INFO)


  parser = argparse.ArgumentParser(description='Script for performing Wi-SUN provisioning.')
  parser.add_argument('--soc', action='store', required=True, help='SoC type')
  parser.add_argument('--jlink_host', action='store', default=None, help='Host name or IP address of J-Link adapter')
  parser.add_argument('--jlink_ser', action='store', default=None, help='Serial number of J-Link adapter')
  subparsers = parser.add_subparsers(help='sub-command help')

  initNvm = subparsers.add_parser('init_nvm',  help='TODO')
  initNvm.add_argument('--prov_img', action='store', required=True, help='Input file for provisiong application')
  initNvm.set_defaults(func=_initNVM)

  genDevKey = subparsers.add_parser('gen_device_key', help='TODO')
  genDevKey.add_argument('--prov_img', action='store', required=True, help='Input file for provisiong application')
  genDevKey.set_defaults(func=_genKey)

  genCSRKey = subparsers.add_parser('gen_csr_key',  help='TODO')
  genCSRKey.add_argument('--prov_img', action='store', required=True, help='Input file for provisiong application')
  genCSRKey.set_defaults(func=_genCSR)

  writeDeviceCerts = subparsers.add_parser('write_device_certs', help='TODO')
  writeDeviceCerts.add_argument('--data', action='store', required=True, help='TODO')
  writeDeviceCerts.add_argument('--prov_img', action='store', required=True, help='Input file for provisiong application')
  writeDeviceCerts.set_defaults(func=_writeDeviceCerts)

  writeBatch = subparsers.add_parser('write_batch_certs', help='TODO')
  writeBatch.add_argument('--data', action='store', required=True, help='TODO')
  writeBatch.add_argument('--prov_img', action='store', required=True, help='Input file for provisiong application')
  writeBatch.set_defaults(func=_writeBatchCerts)

  writeRoot = subparsers.add_parser('write_root_certs', help='TODO')
  writeRoot.add_argument('--data', action='store', required=True, help='TODO')
  writeRoot.add_argument('--prov_img', action='store', required=True, help='Input file for provisiong application')
  writeRoot.set_defaults(func=_writeRootCerts)

  args = parser.parse_args()
  args.func(args)
