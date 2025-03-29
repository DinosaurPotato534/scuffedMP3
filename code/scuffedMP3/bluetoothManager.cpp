#include "bluetoothManager.h"

BluetoothManager* BluetoothManager::_instance = nullptr;

BluetoothManager::BluetoothManager() {
  _scanning = false;
  _connected = false;
  _instance = this;
}

bool BluetoothManager::begin() {
  _a2dp_source.set_stream_reader(_staticDataCallback);

  _a2dp_source.set_on_connection_state_changed([this](esp_a2d_connection_state_t state, void *p_param) {
    if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
      _connected = true;
      Serial.println("Bluetooth connection established");
    } else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
      _connected = false;
      _connectedDevice = "";
      Serial.println("Bluetooth disconnected");
    }
  });
  
  bool result = _a2dp_source.start(DEVICE_NAME);
  if (result) {
    Serial.println("Bluetooth initialized successfully");
  } else {
    Serial.println("Failed to initialize Bluetooth");
  }
  return result;
}

bool BluetoothManager::startScan() {
  if (_scanning) return true;
  
  _devices.clear();
  _scanning = true;
  _scanStartTime = millis();
  
  bool result = _a2dp_source.startScan(scanCallback);
  if (result) {
    Serial.println("Started BT scan");
  } else {
    Serial.println("Failed to start BT scan");
    _scanning = false;
  }
  return result;
}

void BluetoothManager::stopScan() {
  if (!_scanning) return;
  
  _a2dp_source.stopScan();
  _scanning = false;
  Serial.println("Stopped BT scan");
}

bool BluetoothManager::isScanning() {
  if (_scanning && (millis() - _scanStartTime > BT_SCAN_TIMEOUT)) {
    stopScan();
  }
  return _scanning;
}

bool BluetoothManager::connect(int deviceIndex) {
  if (deviceIndex < 0 || deviceIndex >= _devices.size()) {
    return false;
  }
  
  return connectToAddress(_devices[deviceIndex].address);
}

bool BluetoothManager::connectToAddress(String address) {
  if (_connected) {
    disconnect();
  }

  esp_bd_addr_t bda;
  sscanf(address.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", 
         &bda[0], &bda[1], &bda[2], &bda[3], &bda[4], &bda[5]);
  
  bool result = _a2dp_source.connect(bda);
  
  if (result) {
    for (const auto& device : _devices) {
      if (device.address == address) {
        _connectedDevice = device.name;
        break;
      }
    }
    
    Serial.println("Connection initiated to " + _connectedDevice);
  } else {
    Serial.println("Failed to connect to device");
  }
  
  return result;
}

void BluetoothManager::disconnect() {
  if (_connected) {
    _a2dp_source.disconnect();
    _connected = false;
    _connectedDevice = "";
    Serial.println("Disconnected from BT device");
  }
}

bool BluetoothManager::isConnected() {
  return _connected;
}

int BluetoothManager::getDeviceCount() {
  return _devices.size();
}

BTDevice BluetoothManager::getDevice(int index) {
  if (index < 0 || index >= _devices.size()) {
    return {"Invalid", "00:00:00:00:00:00"};
  }
  return _devices[index];
}

String BluetoothManager::getConnectedDeviceName() {
  return _connectedDevice;
}

void BluetoothManager::scanCallback(const uint8_t* addr, int rssi, const char* name) {
  if (_instance) {
    _instance->addDevice(addr, name);
  }
}

void BluetoothManager::addDevice(const uint8_t* addr, const char* name) {
  char addrStr[18];
  sprintf(addrStr, "%02x:%02x:%02x:%02x:%02x:%02x", 
          addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  for (const auto& device : _devices) {
    if (device.address == addrStr) {
      return;
    }
  }
  
  String deviceName = name ? String(name) : "Unknown";
  _devices.push_back({deviceName, String(addrStr)});
  
  Serial.println("Found device: " + deviceName + " (" + String(addrStr) + ")");
}

void BluetoothManager::setDataCallback(DataCallback callback) {
  _dataCallback = callback;
}

int32_t BluetoothManager::_staticDataCallback(uint8_t* data, int32_t len) {
  if (_instance && _instance->_dataCallback) {
    return _instance->_dataCallback(data, len);
  }

  memset(data, 0, len);
  return len;
}
