from thermal_report.bootstrap import prepare_environment

prepare_environment()
from thermal_report.gui import Application

if __name__ == "__main__":
    Application().mainloop()
