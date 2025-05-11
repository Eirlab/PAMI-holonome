import 'dart:async';
import 'dart:convert';
import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:permission_handler/permission_handler.dart';

import 'DiscoveryPage.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Joystick Demo',
      home: JoystickPage(),
    );
  }
}

class JoystickPage extends StatefulWidget {
  @override
  _JoystickPageState createState() => _JoystickPageState();
}

class _JoystickPageState extends State<JoystickPage> {
  Offset _position = Offset.zero;
  final double _joystickRadius = 150;
  final double _knobRadius = 30;

  Offset _center = Offset.zero;
  bool _isTouchingJoystick = false;

  bool ledOn = false;
  bool hornOn = false;
  bool obstacleAvoidance = false;
  String mode = 'headless';
  bool bluetoothConnected = false;

  BluetoothConnection? connection;

  Timer? _joystickTimer;

  @override
  void dispose() {
    connection?.dispose();
    _joystickTimer?.cancel();
    super.dispose();
  }

  Future<void> _requestBluetoothPermissions() async {
    await [
      Permission.bluetooth,
      Permission.bluetoothScan,
      Permission.bluetoothConnect,
      Permission.location,
    ].request();
  }

  Future<void> _connectToBluetoothDevice() async {
    if (bluetoothConnected && connection != null && connection!.isConnected) {
      await connection!.close();
      connection = null;
      setState(() => bluetoothConnected = false);
      print("Déconnecté du Bluetooth");
      return;
    }

    await _requestBluetoothPermissions();

    BluetoothDevice? selectedDevice = await Navigator.of(context).push(
      MaterialPageRoute(builder: (context) => DiscoveryPage()),
    );

    if (selectedDevice != null) {
      try {
        BluetoothConnection.toAddress(selectedDevice.address).then((uconnection) {
          connection = uconnection;
          setState(() => bluetoothConnected = true);

          connection!.input?.listen((data) {
            print('Data received: ${ascii.decode(data)}');
          }).onDone(() {
            print('Disconnected by remote request');
            setState(() => bluetoothConnected = false);
          });
        });
      } catch (e) {
        print('Cannot connect, exception occurred');
        print(e);
      }
    }
  }

  void sendMessage(String message) {
    if (connection != null && connection!.isConnected) {
      connection!.output.add(Uint8List.fromList(utf8.encode(message + "\n")));
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.lightBlue[100],
      body: Column(
        children: [
          const SizedBox(height: 80),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              Row(
                children: [
                  Column(
                    children: [
                      Icon(Icons.lightbulb_outline, size: 40),
                      const Text('LED'),
                    ],
                  ),
                  Switch(
                    value: ledOn,
                    onChanged: (value) {
                      setState(() => ledOn = value);
                      sendMessage(ledOn ? "LED_ON" : "LED_OFF");
                    },
                  ),
                ],
              ),
              ElevatedButton(
                style: ElevatedButton.styleFrom(
                  padding: const EdgeInsets.all(16),
                  shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(12),
                  ),
                ),
                onPressed: () {
                  sendMessage("HORN");
                },
                child: const Icon(Icons.volume_up, size: 32),
              ),
            ],
          ),
          const SizedBox(height: 50),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              ElevatedButton(
                style: ElevatedButton.styleFrom(
                  padding: const EdgeInsets.all(16),
                  shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(12),
                  ),
                ),
                onPressed: () {
                  setState(() => obstacleAvoidance = !obstacleAvoidance);
                  sendMessage(obstacleAvoidance ? "OBSTACLE_ON" : "OBSTACLE_OFF");
                },
                child: Column(
                  children: [
                    ColorFiltered(
                      colorFilter: ColorFilter.mode(
                        obstacleAvoidance ? Colors.blue : Colors.white30,
                        BlendMode.srcIn,
                      ),
                      child: Image.asset(
                        'assets/images/obstacle_icon.png',
                        width: 50,
                        height: 50,
                      ),
                    ),
                    const SizedBox(height: 4),
                    Text(
                      " Obstacle\navoidance",
                      style: TextStyle(
                        color: Colors.blue[900],
                        fontSize: 12,
                        fontWeight: FontWeight.w600,
                      ),
                    ),
                  ],
                ),
              ),
              Container(
                decoration: BoxDecoration(
                  border: Border.all(color: Colors.blue[900]!, width: 2),
                  borderRadius: BorderRadius.circular(16),
                ),
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    GestureDetector(
                      onTap: () {
                        setState(() => mode = 'headless');
                        sendMessage("MODE_HEADLESS");
                      },
                      child: Container(
                        decoration: BoxDecoration(
                          color: mode == 'headless' ? Colors.lightBlue[300] : Colors.lightBlue[100],
                          borderRadius: const BorderRadius.vertical(top: Radius.circular(14)),
                        ),
                        padding: const EdgeInsets.symmetric(horizontal: 32, vertical: 14),
                        child: Text("  Headless ", style: TextStyle(color: Colors.blue[900], fontWeight: FontWeight.bold)),
                      ),
                    ),
                    GestureDetector(
                      onTap: () {
                        setState(() => mode = 'differential');
                        sendMessage("MODE_DIFFERENTIAL");
                      },
                      child: Container(
                        decoration: BoxDecoration(
                          color: mode == 'differential' ? Colors.lightBlue[300] : Colors.lightBlue[100],
                          borderRadius: const BorderRadius.vertical(bottom: Radius.circular(14)),
                        ),
                        padding: const EdgeInsets.symmetric(horizontal: 32, vertical: 14),
                        child: Text("Differential", style: TextStyle(color: Colors.blue[900], fontWeight: FontWeight.bold)),
                      ),
                    ),
                  ],
                ),
              ),
            ],
          ),
          const SizedBox(height: 50),
          GestureDetector(
            onTap: _connectToBluetoothDevice,
            child: Container(
              padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 14),
              decoration: BoxDecoration(
                color: bluetoothConnected ? Colors.green : Colors.red,
                borderRadius: BorderRadius.circular(20),
              ),
              child: Text(
                bluetoothConnected ? "Bluetooth Connecté" : "Connecter Bluetooth",
                style: const TextStyle(color: Colors.white, fontSize: 16),
              ),
            ),
          ),
          const Spacer(),
          Center(
            child: LayoutBuilder(
              builder: (context, constraints) {
                return GestureDetector(
                  onPanStart: (details) {
                    RenderBox renderBox = context.findRenderObject() as RenderBox;
                    _center = renderBox.localToGlobal(Offset.zero) + Offset(_joystickRadius, _joystickRadius);

                    Offset touchPoint = details.globalPosition;
                    Offset delta = touchPoint - _center;
                    if (delta.distance <= _joystickRadius) {
                      _isTouchingJoystick = true;
                      setState(() => _position = _clampOffset(delta));

                      _joystickTimer = Timer.periodic(Duration(milliseconds: 50), (_) {
                        if (_isTouchingJoystick) {
                          _sendJoystickData(_position);
                        }
                      });
                    }
                  },
                  onPanUpdate: (details) {
                    if (_isTouchingJoystick) {
                      Offset touchPoint = details.globalPosition;
                      Offset delta = touchPoint - _center;
                      setState(() => _position = _clampOffset(delta));
                      _sendJoystickData(_position);
                    }
                  },
                  onPanEnd: (details) {
                    _isTouchingJoystick = false;
                    _joystickTimer?.cancel();
                    _joystickTimer = null;
                    setState(() => _position = Offset.zero);
                    _sendJoystickData(_position);
                  },
                  child: Container(
                    width: _joystickRadius * 2,
                    height: _joystickRadius * 2,
                    decoration: BoxDecoration(
                      color: Colors.lightBlue[300],
                      shape: BoxShape.circle,
                    ),
                    child: Stack(
                      children: [
                        Positioned(
                          left: _joystickRadius + _position.dx - _knobRadius,
                          top: _joystickRadius + _position.dy - _knobRadius,
                          child: Container(
                            width: _knobRadius * 2,
                            height: _knobRadius * 2,
                            decoration: BoxDecoration(
                              color: Colors.blue[800],
                              shape: BoxShape.circle,
                            ),
                          ),
                        ),
                      ],
                    ),
                  ),
                );
              },
            ),
          ),
          const SizedBox(height: 30),
        ],
      ),
    );
  }

  Offset _clampOffset(Offset offset) {
    if (offset.distance <= _joystickRadius - _knobRadius) {
      return offset;
    } else {
      return Offset.fromDirection(offset.direction, _joystickRadius - _knobRadius);
    }
  }

  void _sendJoystickData(Offset offset) {
    final angle = (offset.direction * 180 / 3.14 + 90) % 360;
    final normalizedDistance = offset.distance / (_joystickRadius - _knobRadius);
    final distance255 = (normalizedDistance.clamp(0.0, 1.0) * 255).round();
    sendMessage("JOY:${angle.toStringAsFixed(0)},$distance255");
  }
}
