#ifndef BLUETOOTH_MANAGER_H
#define BLUETOOTH_MANAGER_H

#include "config.h"
#include <BluetoothA2DPSource.h>
#include <vector>
#include <functional>

typedef std::function<int32_t(uint8_t*, int32_t)> DataCallback;

struct BTDevice {
  String name;
  String address;
};

class BluetoothManager {
  public:
    BluetoothManager();
    bool begin();
    bool startScan();
    void stopScan();
    bool isScanning();
    bool connect(int deviceIndex);
    bool connectToAddress(String address);
    void disconnect();
    bool isConnected();
    int getDeviceCount();
    BTDevice getDevice(int index);
    String getConnectedDeviceName();
    void setDataCallback(DataCallback callback);
    int32_t getData(uint8_t* data, int32_t len);
    
  private:
    BluetoothA2DPSource _a2dp_source;
    std::vector<BTDevice> _devices;
    bool _scanning;
    bool _connected;
    String _connectedDevice;
    unsigned long _scanStartTime;
    DataCallback _dataCallback;
    
    static void scanCallback(const uint8_t* addr, int rssi, const char* name);
    void addDevice(const uint8_t* addr, const char* name);
    static BluetoothManager* _instance;

    static int32_t _staticDataCallback(uint8_t* data, int32_t len);
};

#endif // BLUETOOTH_MANAGER_H
