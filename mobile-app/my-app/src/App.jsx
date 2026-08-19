import { useState, useRef, useCallback } from 'react'
import './App.css'

// Same UUIDs as cosplay_prop_ble_full.ino - keep these in sync if you change the firmware.
// If a UUID here doesn't exactly match the #define in the .ino, getCharacteristic()
// below will throw "characteristic not found" - that's the #1 thing to check if
// connect succeeds but a specific control silently does nothing.
const SERVICE_UUID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b'
const BRIGHTNESS_CHAR_UUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8'
const ANIM_MODE_CHAR_UUID = '0a3b0f1e-6b0a-4a6a-9a3a-3e6b1a2c4d5e'
const HEART_RATE_CHAR_UUID = '6b3f1a4c-2d5e-4f6a-8b9c-1a2b3c4d5e6f'
const COLOR_CHAR_UUID = '9c1e2f3a-4b5c-4d6e-8f7a-1b2c3d4e5f6a'
const HR_INPUT_CHAR_UUID = '2d3e4f5a-6b7c-4d8e-9f0a-1b2c3d4e5f6b'
const MOTION_INPUT_CHAR_UUID = '5a6b7c8d-9e0f-4a1b-8c2d-3e4f5a6b7c8d'

const COLORS = ['#ffffff', '#ff3b30', '#ff9500', '#ffcc00', '#34c759', '#0a84ff', '#af52de', '#ff2d92']

const ANIM_MODES = [
  { label: 'Heartbeat pulse', value: 'pulse' },
  { label: 'Slash sweep', value: 'slash' },
]

function BluetoothIcon({ connected }) {
  return (
    <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke={connected ? '#34c759' : '#666'} strokeWidth="2">
      <path d="M6.5 6.5l11 11L12 23V1l5.5 5.5-11 11" strokeLinecap="round" strokeLinejoin="round" />
    </svg>
  )
}

function HeartIcon() {
  return (
    <svg width="28" height="28" viewBox="0 0 24 24" fill="#9a9a9a">
      <path d="M12 21s-6.7-4.35-9.3-8.1C.9 10.1 1.4 6.6 4.2 5c2-1.15 4.4-.55 5.7 1.1.4.5.7 1 1.1 1.6.4-.6.7-1.1 1.1-1.6C13.4 4.45 15.8 3.85 17.8 5c2.8 1.6 3.3 5.1 1.5 7.9C16.7 16.65 12 21 12 21z" />
    </svg>
  )
}

function WalkingIcon() {
  return (
    <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="#9a9a9a" strokeWidth="1.8" strokeLinecap="round" strokeLinejoin="round">
      <circle cx="13.5" cy="4" r="1.8" fill="#9a9a9a" stroke="none" />
      <path d="M11 8l3 1.5 2 4M14 9.5l-1.5 4-3.5 2M14 9.5l2.5.5 2 3.5M8.5 21l2-5.5-1-3" />
    </svg>
  )
}

export default function App() {
  const [connected, setConnected] = useState(false)
  const [connecting, setConnecting] = useState(false)
  const [error, setError] = useState('')
  const [heartRate, setHeartRate] = useState(null)
  const [motionState, setMotionState] = useState('Idle')
  const [brightness, setBrightness] = useState(50)
  const [selectedColor, setSelectedColor] = useState(COLORS[0])
  const [animMode, setAnimMode] = useState('pulse')
  const [heartRateInput, setHeartRateInput] = useState(true)
  const [motionInput, setMotionInput] = useState(true)

  // Refs (not state) because these hold live BLE objects, not UI data -
  // storing them as state would cause pointless re-renders every time
  // we grab a new characteristic during connect.
  const brightnessCharRef = useRef(null)
  const animModeCharRef = useRef(null)
  const colorCharRef = useRef(null)
  const hrInputCharRef = useRef(null)
  const motionInputCharRef = useRef(null)
  const deviceRef = useRef(null)

  const handleConnect = useCallback(async () => {
    setError('')
    if (!navigator.bluetooth) {
      setError('Web Bluetooth is not available in this browser. Use Chrome on Android or desktop.')
      return
    }

    setConnecting(true)
    try {
      const device = await navigator.bluetooth.requestDevice({
        filters: [{ services: [SERVICE_UUID] }],
        optionalServices: [SERVICE_UUID],
      })
      deviceRef.current = device
      device.addEventListener('gattserverdisconnected', () => setConnected(false))

      const server = await device.gatt.connect()
      const service = await server.getPrimaryService(SERVICE_UUID)

      // Grab every characteristic up front, once, right after connecting -
      // each of these fails loudly (throws, caught below) if a UUID here
      // doesn't match one the ESP32 actually created, which is the
      // fastest way to catch a copy-paste UUID mismatch during testing.
      brightnessCharRef.current = await service.getCharacteristic(BRIGHTNESS_CHAR_UUID)
      animModeCharRef.current = await service.getCharacteristic(ANIM_MODE_CHAR_UUID)
      colorCharRef.current = await service.getCharacteristic(COLOR_CHAR_UUID)
      hrInputCharRef.current = await service.getCharacteristic(HR_INPUT_CHAR_UUID)
      motionInputCharRef.current = await service.getCharacteristic(MOTION_INPUT_CHAR_UUID)
      const heartRateChar = await service.getCharacteristic(HEART_RATE_CHAR_UUID)

      await heartRateChar.startNotifications()
      heartRateChar.addEventListener('characteristicvaluechanged', (event) => {
        const bpm = event.target.value.getUint8(0)
        setHeartRate(bpm)
        setMotionState(bpm > 0 ? 'Reading' : 'Idle')
      })

      setConnected(true)
    } catch (err) {
      setError(err.message || 'Connection failed or was cancelled.')
    } finally {
      setConnecting(false)
    }
  }, [])

  const handleDisconnect = useCallback(() => {
    if (deviceRef.current?.gatt?.connected) {
      deviceRef.current.gatt.disconnect()
    }
    setConnected(false)
    setHeartRate(null)
  }, [])

  const handleBrightnessChange = async (value) => {
    setBrightness(value)
    if (brightnessCharRef.current) {
      const byteValue = Math.round((value / 100) * 255)
      try {
        await brightnessCharRef.current.writeValue(new Uint8Array([byteValue]))
      } catch (err) {
        setError('Failed to write brightness: ' + err.message)
      }
    }
  }

  const handleAnimModeChange = async (value) => {
    setAnimMode(value)
    if (animModeCharRef.current) {
      try {
        await animModeCharRef.current.writeValue(new TextEncoder().encode(value))
      } catch (err) {
        setError('Failed to write animation mode: ' + err.message)
      }
    }
  }

  // Converts a "#rrggbb" hex string (what the swatches store) into the
  // 3-byte [R, G, B] array the firmware's ColorCallbacks expects.
  const hexToRgbBytes = (hex) => {
    const clean = hex.replace('#', '')
    const r = parseInt(clean.substring(0, 2), 16)
    const g = parseInt(clean.substring(2, 4), 16)
    const b = parseInt(clean.substring(4, 6), 16)
    return new Uint8Array([r, g, b])
  }

  const handleColorSelect = async (color) => {
    setSelectedColor(color)
    if (colorCharRef.current) {
      try {
        await colorCharRef.current.writeValue(hexToRgbBytes(color))
      } catch (err) {
        setError('Failed to write color: ' + err.message)
      }
    }
  }

  const handleHeartRateInputToggle = async (checked) => {
    setHeartRateInput(checked)
    if (hrInputCharRef.current) {
      try {
        await hrInputCharRef.current.writeValue(new Uint8Array([checked ? 1 : 0]))
      } catch (err) {
        setError('Failed to write heart rate input toggle: ' + err.message)
      }
    }
  }

  const handleMotionInputToggle = async (checked) => {
    setMotionInput(checked)
    if (motionInputCharRef.current) {
      try {
        await motionInputCharRef.current.writeValue(new Uint8Array([checked ? 1 : 0]))
      } catch (err) {
        setError('Failed to write motion input toggle: ' + err.message)
      }
    }
  }

  return (
    <div className="phone">
      <div className="header">
        <h1>
          <span className="header-bold">Cosplay</span> <span className="header-light">Orfevre</span>
        </h1>
        <button className="conn-pill" onClick={connected ? handleDisconnect : handleConnect} disabled={connecting}>
          <BluetoothIcon connected={connected} />
          <span style={{ color: connected ? '#34c759' : '#888' }}>
            {connecting ? 'Connecting...' : connected ? 'Connected' : 'Disconnected'}
          </span>
        </button>
      </div>

      {error && <div className="error-banner">{error}</div>}

      <div className="stat-row">
        <div className="stat-card">
          <HeartIcon />
          <div className="stat-value">
            {heartRate ?? '--'} <span className="stat-unit">bpm</span>
          </div>
        </div>
        <div className="stat-card">
          <WalkingIcon />
          <div className="stat-value stat-value-text">{motionState}</div>
        </div>
      </div>

      <div className="section">
        <div className="section-label-row">
          <span className="section-label">Brightness</span>
          <span className="section-value">{brightness}%</span>
        </div>
        <input
          type="range"
          min="0"
          max="100"
          value={brightness}
          onChange={(e) => handleBrightnessChange(Number(e.target.value))}
          className="slider"
        />
      </div>

      <div className="section">
        <span className="section-label">Color</span>
        <div className="color-row">
          {COLORS.map((color) => (
            <button
              key={color}
              className="swatch"
              style={{ background: color, borderColor: selectedColor === color ? '#fff' : 'transparent' }}
              onClick={() => handleColorSelect(color)}
              aria-label={`Select color ${color}`}
            />
          ))}
        </div>
      </div>

      <div className="section">
        <span className="section-label">Animation Mode</span>
        <select className="dropdown" value={animMode} onChange={(e) => handleAnimModeChange(e.target.value)}>
          {ANIM_MODES.map((mode) => (
            <option key={mode.value} value={mode.value}>
              {mode.label}
            </option>
          ))}
        </select>
      </div>

      <div className="toggle-row">
        <span className="section-label">Heart Rate Input</span>
        <input
          type="checkbox"
          checked={heartRateInput}
          onChange={(e) => handleHeartRateInputToggle(e.target.checked)}
        />
      </div>
      <div className="toggle-row">
        <span className="section-label">Motion Input</span>
        <input type="checkbox" checked={motionInput} onChange={(e) => handleMotionInputToggle(e.target.checked)} />
      </div>
    </div>
  )
}
