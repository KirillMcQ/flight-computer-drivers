import serial
import time
import pyvista



pl = pyvista.Plotter()
rocket = pyvista.read("./tools/visualizer/rocket.stl")
actor = pl.add_mesh(rocket)
pl.add_axes()

pl.camera_position = [
    (1500, 0, 300),
    (0, 0, 300),
    (0, 0, 1)
]

pl.show(interactive_update = True)

didConnectToFlightComputer = False

while pl.iren.initialized:
      if not didConnectToFlightComputer:
          try:
              ser = serial.Serial('/dev/tty.usbserial-140', 115200, timeout=1)
              didConnectToFlightComputer = True
              time.sleep(2)
          except:
              time.sleep(0.01)
              pl.update()
              continue
      try:
        if ser.in_waiting > 0:
          line = ser.readline().decode('utf-8').strip()
          if line.startswith("Calibrating"):
              pl.add_text(line + ", please wait", name = "Calibration Message", font_size = 36, position = "upper_right")
          else:
            pl.remove_actor("Calibration Message")
            yaw, pitch, roll = [float(i) for i in line.split(",")] # Degrees
            print(yaw, pitch, roll)
            actor.orientation = (-yaw, pitch, -roll)
      except:
          didConnectToFlightComputer = False

      pl.update()
      time.sleep(0.01)