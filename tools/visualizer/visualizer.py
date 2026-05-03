import serial
import time
import pyvista

ser = serial.Serial('/dev/tty.usbserial-140', 115200, timeout=1)
time.sleep(2)

pl = pyvista.Plotter()
rocket = pyvista.read("./tools/visualizer/rocket.stl")
actor = pl.add_mesh(rocket)
pl.add_axes()

pl.show(interactive_update = True)

while pl.iren.initialized:
      if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').strip()
        yaw, pitch, roll = [float(i) for i in line.split(",")] # Degrees
        print(yaw, pitch, roll)
        actor.orientation = (-pitch, -yaw, -roll)

      pl.update()
      time.sleep(0.01)