import serial
import time
import pyvista
import numpy

pl = pyvista.Plotter()
rocket = pyvista.read("rocket2.stl")
actor = pl.add_mesh(rocket)
pl.add_axes()

pl.camera_position = [(1500, 0, 100), (0, 0, 100), (0, 0, 1)]
pl.camera.clipping_range = (1, 10000)


times = []
yawData = []
pitchData = []
rollData = []

startTime = time.time()

yawChart = pyvista.Chart2D(
    size=(0.3, 0.3),
    loc=(0.65, 0.60),
    x_label="Time (s)",
    y_label="Yaw Angle (deg)"
)

pitchChart = pyvista.Chart2D(
    size=(0.3, 0.3),
    loc=(0.65, 0.30),
    x_label="Time (s)",
    y_label="Pitch Angle (deg)"
)

rollChart = pyvista.Chart2D(
    size=(0.3, 0.3),
    loc=(0.65, 0.04),
    x_label="Time (s)",
    y_label="Roll Angle (deg)"
)

yawChart.title = "Yaw"
yawChart.y_range = [-180, 180]
yawChart.legend_visible = True
yawChart.background_color = (0.9, 0.9, 0.9)

pitchChart.title = "Pitch"
pitchChart.y_range = [-180, 180]
pitchChart.legend_visible = True
pitchChart.background_color = (0.9, 0.9, 0.9)

rollChart.title = "Roll"
rollChart.y_range = [-180, 180]
rollChart.legend_visible = True
rollChart.background_color = (0.9, 0.9, 0.9)

yawLine = yawChart.line([0], [0], color="r", label="Yaw")
pitchLine = pitchChart.line([0], [0], color="g", label="Pitch")
rollLine = rollChart.line([0], [0], color="b", label="Roll")


pl.add_chart(yawChart)
pl.add_chart(pitchChart)
pl.add_chart(rollChart)

pl.show(interactive_update = True)

didConnectToFlightComputer = False

while pl.iren.initialized:
      if not didConnectToFlightComputer:
          try:
              ser = serial.Serial('/dev/tty.usbserial-140', 115200, timeout=1)
              didConnectToFlightComputer = True
              pl.remove_actor("notConnectedMessage")
              time.sleep(2)
          except:
              pl.add_text("Flight Computer Not Connected", name = "notConnectedMessage", font_size = 36, position = "upper_right")
              time.sleep(0.01)
              pl.update()
              continue
      try:
        if ser.in_waiting > 0:
          line = ser.readline().decode('utf-8').strip()
          if line.startswith("Calibrating"):
              pl.add_text(line + ", please wait", name = "Calibration Message", font_size = 36, position = "upper_right")
          elif not line:
              pl.add_text("Loading", name = "loadingMessage", font_size = 36, position = "upper_right")
          else:
            pl.remove_actor("loadingMessage")
            pl.remove_actor("Calibration Message")
            yaw, pitch, roll = [float(i) for i in line.split(",")] # Degrees
            actor.orientation = (-yaw, pitch, -roll)

            t = time.time() - startTime

            times.append(t)
            yawData.append(yaw)
            pitchData.append(pitch)
            rollData.append(roll)

            if len(times) > 200:
                times = times[-200:]
                yawData = yawData[-200:]
                pitchData = pitchData[-200:]
                rollData = rollData[-200:]

            x = numpy.array(times)
            yawArr = numpy.array(yawData)
            pitchArr = numpy.array(pitchData)
            rollArr = numpy.array(rollData)

            yawLine.update(x, yawArr)
            pitchLine.update(x, pitchArr)
            rollLine.update(x, rollArr)
      except:
          didConnectToFlightComputer = False

      pl.update()
      time.sleep(0.01)