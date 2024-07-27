import sys
from PyQt5.QtWidgets import QWidget, QApplication, QMainWindow
from PyQt5.QtCore import QTimer  # Import QTimer
from mainwindow import Ui_MainWindow
import simpleio


BlueLedSts = 0
RedLedSts = 0
GreenLedSts = 0
YellowLedSts = 0

class MyApp(QMainWindow):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        # Create a QTimer object
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.readUSBStatus)
        self.timer.start(3000)  # timer for 3sec

        # UI alignment
        self.ui.ledAllOnButton.clicked.connect(self.ledAllOn)
        self.ui.ledAllOffButtion.clicked.connect(self.ledAllOff)
        self.ui.RC0_LED_ON.clicked.connect(self.BuleLedOn)
        self.ui.RC0_LED_OFF.clicked.connect(self.BuleLedOff)
        self.ui.RC1_LED_ON.clicked.connect(self.RedLedOn)
        self.ui.RC1_LED_OFF.clicked.connect(self.RedLedOff)
        self.ui.RC2_LED_ON.clicked.connect(self.GreenLedOn)
        self.ui.RC2_LED_OFF.clicked.connect(self.GreenLedOff)
        self.ui.RC3_LED_ON.clicked.connect(self.YellowLedOn)
        self.ui.RC3_LED_OFF.clicked.connect(self.YellowLedOff)
        
        self.ui.sw1StsButton.clicked.connect(self.sw1ReadSts)

    def ledAllOn(self):
        global BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts
        BlueLedSts = 1
        RedLedSts = 1
        GreenLedSts = 1
        YellowLedSts = 1
        simpleio.usb_led(BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts)

    def ledAllOff(self):
        global BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts
        BlueLedSts = 0
        RedLedSts = 0
        GreenLedSts = 0
        YellowLedSts = 0
        simpleio.usb_led(BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts)

    def BuleLedOn(self):
        global BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts
        BlueLedSts = 1
        simpleio.usb_led(BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts)

    def BuleLedOff(self):
        global BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts
        BlueLedSts = 0
        simpleio.usb_led(BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts)
    
    def RedLedOn(self):
        global BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts
        RedLedSts = 1
        simpleio.usb_led(BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts)
    
    def RedLedOff(self):
        global BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts
        RedLedSts = 0
        simpleio.usb_led(BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts)
    
    def GreenLedOn(self):
        global BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts
        GreenLedSts = 1
        simpleio.usb_led(BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts)

    def GreenLedOff(self):
        global BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts
        GreenLedSts = 0
        simpleio.usb_led(BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts)
    
    def YellowLedOn(self):
        global BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts
        YellowLedSts = 1
        simpleio.usb_led(BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts)
    
    def YellowLedOff(self):
        global BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts
        YellowLedSts = 0
        simpleio.usb_led(BlueLedSts, RedLedSts, GreenLedSts, YellowLedSts)

    def sw1ReadSts(self):
        read_data = simpleio.usb_status()
        # Convert the bytearray to a string and set it as the label text
        text_to_display = " ".join(format(byte, "02x") for byte in read_data)
        self.ui.swLabel.setText(text_to_display)
     
    def readUSBStatus(self):
        read_data = simpleio.usb_status()
        # Convert the bytearray to a string and set it as the label text
        text_to_display = " ".join(format(byte, "02x") for byte in read_data)
        self.ui.sw2Label.setText(text_to_display)
        self.ui.sw2Label_2.setText(text_to_display)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    myapp = MyApp()
    myapp.show()
    sys.exit(app.exec_())