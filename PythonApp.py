import customtkinter as ctk
import CTkMessagebox
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.animation as animation
import serial
import threading
import time
import hashlib
import random
import serial.tools.list_ports

# Configuración inicial
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

INTERVALO_ACTUALIZACION = 100  # Intervalo de actualización del gráfico en ms

caracter_escalon = '(C4$)'

class TemperatureControlGUI(ctk.CTk):
    def __init__(self):
        super().__init__()
        
        # Configuración de la ventana
        self.title("Control de Temperatura - Modern GUI")
        self.geometry("1100x750")
        
        # Configuración serial
        self.serial_port = self.find_serial_port()
        self.baudrate = 9600
        
        self.bytesize=serial.EIGHTBITS,  # Formato de bits
        self.parity=serial.PARITY_NONE,  # Sin paridad
        self.stopbits=serial.STOPBITS_ONE,  # 1 bit de stop
        self.timeout=1  # Tiempo de espera para recibir datos
        
        self.ser = None
        self.reading_data = True
        self.eeprom_checked = False
        self.simulation_mode = ctk.BooleanVar(value=False)

        # Variables de control (nuevas variables para mostrar valores)
        self.current_setpoint = ctk.StringVar(value="setpoint: 30.0 °C")
        self.current_hysteresis = ctk.StringVar(value="hysteresis: 2.0 °C")
        self.current_interval = ctk.StringVar(value="interval: 1000 ms")

        # Variables de control
        self.current_temperature = ctk.StringVar(value="temperatura: - - °C")
        self.setpoint = ctk.DoubleVar(value=30.0)
        self.hysteresis = ctk.DoubleVar(value=2.0)
        self.sampling_interval = ctk.IntVar(value=1000)
        self.connection_status = ctk.StringVar(value="Modo Simulación")
        self.heater_state = ctk.BooleanVar(value=False)

        self.flag_aplicar_cambios = True
        self.setpoint_temp = 30.0
        self.hysteresis_temp = 2.0

        # Variables de datos
        
        self.simulation_start_time = time.time()
        self.last_sample_time = 0
        self.x_real = []
        self.y_real = []
        
        self.last_exclamation_time = self.simulation_start_time

        # Configuración del layout
        self.grid_columnconfigure(0, weight=0, minsize=300)
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)

        # Paneles
        self.left_panel = ctk.CTkFrame(self, width=300)
        self.left_panel.grid(row=0, column=0, sticky="ns", padx=10, pady=10)
        self.left_panel.grid_propagate(False)
        
        self.right_panel = ctk.CTkFrame(self)
        self.right_panel.grid(row=0, column=1, sticky="nsew", padx=10, pady=10)
        
        # Inicializar componentes
        self.create_left_panel()
        self.create_right_panel()
        
        # Hilo de datos
        self.data_thread = threading.Thread(target=self.data_acquisition)
        self.data_thread.daemon = True
        self.data_thread.start()

        # Inicializar gráfico
        self.init_plot()
        
    def find_serial_port(self):
        """Busca automáticamente el puerto serial disponible"""
        ports = list(serial.tools.list_ports.comports())
        if ports:
            return ports[0].device
        return None

    def create_left_panel(self):
        """Panel de control izquierdo con distintos frames organizados."""
        self.left_panel.grid_columnconfigure(0, weight=1)
        
        # Frame principal contenedor
        main_container = ctk.CTkFrame(self.left_panel, fg_color="transparent")
        main_container.pack(fill="both", expand=True, padx=5, pady=5)
        
        # Sección 1: Ingreso de valores y botón de aplicar cambios
        input_frame = ctk.CTkFrame(main_container, fg_color="transparent")
        input_frame.pack(fill="x", pady=10)
        
        ctk.CTkLabel(input_frame, text="Ingresar Parámetros", font=("Arial", 16)).pack(pady=5)
        
        self.create_input_field(input_frame, "Setpoint (°C):", self.setpoint)
        self.create_input_field(input_frame, "Histéresis (°C):", self.hysteresis)
        self.create_input_field(input_frame, "Intervalo (ms):", self.sampling_interval)
        
        ctk.CTkButton(input_frame, text="Aplicar Parámetros", 
                    command=self.apply_changes, width=280, fg_color="#1f6aa5", hover_color="#145a8d", corner_radius=10).pack(pady=5)
        
        ctk.CTkFrame(main_container, height=2, fg_color="#555555").pack(fill="x", padx=5, pady=5)
        
        # Sección 2: Valores aplicados y estado del calefactor
        status_frame = ctk.CTkFrame(main_container, fg_color="transparent")
        status_frame.pack(fill="x", pady=10)
        
        ctk.CTkLabel(status_frame, text="Valores Eeprom:", font=("Arial", 14)).pack(pady=5)

        # Crear un frame para contener los valores en una fila
        #values_frame = ctk.CTkFrame(status_frame, fg_color="#1e1e1e")
        #values_frame.pack()


        # Setpoint
        self.setpoint_frame = ctk.CTkFrame(status_frame, fg_color="#004B87")
        self.setpoint_frame.pack(pady=5)

        self.lbl_temperatura = ctk.CTkLabel(self.setpoint_frame, textvariable=self.current_setpoint, 
                                        text_color="white", width=280)
        self.lbl_temperatura.pack(side="top")

        # Separador 1
        #ctk.CTkLabel(values_frame, text="|", font=("Arial", 14)).grid(row=0, column=1, padx=10)

        # Hysteresis
        self.hysteresis_frame = ctk.CTkFrame(status_frame, fg_color="#004B87")
        self.hysteresis_frame.pack(pady=5)

        self.lbl_temperatura = ctk.CTkLabel(self.hysteresis_frame, textvariable=self.current_hysteresis, 
                                        text_color="white", width=280)
        self.lbl_temperatura.pack(side="top")

        # Separador 2
        #ctk.CTkLabel(values_frame, text="|", font=("Arial", 14)).grid(row=0, column=3, padx=10)

        # Interval
        self.interval_frame = ctk.CTkFrame(status_frame, fg_color="#004B87")
        self.interval_frame.pack(pady=5)

        self.lbl_temperatura = ctk.CTkLabel(self.interval_frame, textvariable=self.current_interval, 
                                        text_color="white", width=280)
        self.lbl_temperatura.pack(side="top")
        
        #temperatura
        self.temperature_frame = ctk.CTkFrame(status_frame, fg_color="#004B87")
        self.temperature_frame.pack(pady=5)

        self.lbl_temperatura = ctk.CTkLabel(self.temperature_frame, textvariable=self.current_temperature, 
                                        text_color="white", width=280)
        self.lbl_temperatura.pack(side="top")
        
        self.heater_frame = ctk.CTkFrame(status_frame, fg_color="transparent")
        self.heater_frame.pack(pady=5)
        
        self.heater_label = ctk.CTkLabel(self.heater_frame, text="CALEFACTOR: APAGADO", 
                                        text_color="white", width=280)
        self.heater_label.pack(side="top")  # Primero el texto arriba

        self.heater_circle = ctk.CTkLabel(self.heater_frame, text="⬤", text_color="white", 
                                        font=("Arial", 24))
        self.heater_circle.pack(side="top", pady=2)  # Luego el círculo abajo

        self.serial_label = ctk.CTkLabel(self.heater_frame, text="CONEXIÓN SERIE: OFF", 
                                        text_color="white", width=280)
        self.serial_label.pack(side="top")  # Primero el texto arriba

        self.serial_circle = ctk.CTkLabel(self.heater_frame, text="⬤", text_color="white", 
                                        font=("Arial", 24))
        self.serial_circle.pack(side="top", pady=2)  # Luego el círculo abajo

        
        ctk.CTkFrame(main_container, height=2, fg_color="#555555").pack(fill="x", padx=5, pady=5)
        
        # Sección 3: Modo simulación y conexión serie
        control_frame = ctk.CTkFrame(main_container, fg_color="transparent")
        control_frame.pack(fill="x", pady=10)
        
        ctk.CTkSwitch(control_frame, text="Modo Simulación", variable=self.simulation_mode,
                    command=self.toggle_simulation).pack(pady=5)
        
        ctk.CTkButton(control_frame, text="Conectar", 
                    command=self.reconnect, width=280, fg_color="#1f6aa5", hover_color="#145a8d", corner_radius=10).pack(pady=5)
        
        ctk.CTkButton(control_frame, text="Respuesta al escalón", 
                    command=self.generar_escalón, width=280, fg_color="#1f6aa5", hover_color="#145a8d", corner_radius=10).pack(pady=5)

    def generar_escalón(self):
        """Envía el caracter por la terminal que generará en el micro un escalón"""
        if not self.simulation_mode.get() and self.ser and self.ser.is_open:
            self.ser.write(caracter_escalon.encode())

    def create_input_field(self, parent, label, variable):
        """Crea campos de entrada uniformes"""
        frame = ctk.CTkFrame(parent)
        frame.pack(fill="x", padx=5, pady=5)
        ctk.CTkLabel(frame, text=label, width=120).pack(side="left")
        entry = ctk.CTkEntry(frame, textvariable=variable, width=150)
        entry.pack(side="right")

    def create_right_panel(self):
        """Panel del gráfico derecho con switcher de vista"""
        self.fig, self.ax = plt.subplots()
        self.fig.patch.set_facecolor("#2b2b2b")
        self.ax.set_facecolor("#1e1e1e")
        
        # Configurar ambas líneas
        self.line_real, = self.ax.plot([], [], color="#1f77b4", label="Tiempo Real")
        
        # Leyenda en el borde inferior izquierdo
        self.ax.legend(facecolor="#2b2b2b", edgecolor="white", labelcolor="white",
                    loc="lower left", bbox_to_anchor=(0, 0))
        
        self.ax.set_title("Temperatura en Tiempo Real", color="white", pad=20, loc="left")
        self.ax.set_xlabel("Tiempo (s)", color="white")
        self.ax.set_ylabel("Temperatura (°C)", color="white")
        self.ax.tick_params(colors="white")
        self.ax.grid(color="gray", linestyle="--")
        
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.right_panel)
        self.canvas.get_tk_widget().pack(fill="both", expand=True, padx=10, pady=10)
        
        self.ani = animation.FuncAnimation(self.fig, self.update_plot, 
                                        init_func=self.init_plot, 
                                        interval=INTERVALO_ACTUALIZACION, 
                                        blit=False)

    def init_plot(self):
        """Inicializa las líneas del gráfico"""
        self.line_real.set_data([], [])
        return [self.line_real]

    def update_current_values(self):  # NUEVO MÉTODO
        """Actualiza las etiquetas de valores aplicados"""
        self.current_setpoint.set(f"setpoint: {self.setpoint.get()} °C")
        self.current_hysteresis.set(f"hysteresis: {self.hysteresis.get()} °C")
        self.current_interval.set(f"interval: {self.sampling_interval.get()} ms")

    def update_plot(self, frame):
        """Actualiza ambos gráficos pero solo muestra el activo"""
        try:
            # Actualizar datos de ambas líneas
            self.line_real.set_data(self.x_real, self.y_real)
            
            # Centrar el gráfico en el setpoint y mostrar solo 4 veces la histéresis
            if self.flag_aplicar_cambios:
                try:
                    if self.simulation_mode.get():
                        setpoint_val = self.setpoint.get()
                        hysteresis_val = self.hysteresis.get()
                    else:
                        # Parsear valores de las etiquetas EEPROM
                        setpoint_str = self.current_setpoint.get()
                        setpoint_val = float(setpoint_str.split(": ")[1].split()[0])
                        hysteresis_str = self.current_hysteresis.get()
                        hysteresis_val = float(hysteresis_str.split(": ")[1].split()[0])
                    
                    # Ajustar límites del gráfico
                    self.ax.set_ylim(setpoint_val - 4 * hysteresis_val, 
                                setpoint_val + 4 * hysteresis_val)
                    
                    # Limpiar y redibujar líneas de histéresis
                    for line in self.ax.get_lines():
                        if line.get_label() == "hysteresis":
                            line.remove()
                    self.ax.axhline(setpoint_val + hysteresis_val/2, color='lightskyblue', 
                                linestyle="-.", linewidth=0.8, label="hysteresis")
                    self.ax.axhline(setpoint_val - hysteresis_val/2, color='lightskyblue', 
                                linestyle="-.", linewidth=0.8, label="hysteresis")
                    
                    self.flag_aplicar_cambios = False
                except (ValueError, IndexError):
                    pass  # Ignorar errores de parseo iniciales
            
            
            # Autoescalado en el eje X
            if self.x_real:
                self.ax.set_xlim(self.x_real[0], self.x_real[-1])
            
        except Exception as e:
            print(f"Error actualizando gráfico: {str(e)}")
        
        return [self.line_real]

    def data_acquisition(self):
        """Hilo principal de adquisición de datos"""
        while self.reading_data:
            if self.simulation_mode.get():
                self.process_simulation()
            else:
                self.read_serial()
            time.sleep(0.01)

    def process_simulation(self):
        """Procesa datos de simulación"""
        current_time = time.time()
        elapsed = current_time - self.simulation_start_time
        
        if (current_time - self.last_sample_time) >= (self.sampling_interval.get()/1000):
            temp = self.simulate_temperature()
            self.current_temperature.set(f"temperatura: {temp:.1f} °C")
            self.y_real.append(temp)
            self.x_real.append(elapsed)
            self.last_sample_time = current_time
            
            # Mantener buffer circular
            if len(self.x_real) > 100:
                self.x_real.pop(0)
                self.y_real.pop(0)

    def simulate_temperature(self):
        """Genera datos de temperatura simulada con histéresis"""
        setpoint_str = self.current_setpoint.get()
        base = float(setpoint_str.split(": ")[1].split()[0])
        hysteresis_str = self.current_hysteresis.get()
        hyst = float(hysteresis_str.split(": ")[1].split()[0])
        #base = self.setpoint.get()
        #hyst = self.hysteresis.get()
        
        if self.heater_state.get():
            temp = base - hyst/2 + random.uniform(-0.5, 0.5)
            if random.random() < 0.1:  # Probabilidad de apagado
                self.heater_state.set(False)
                self.heater_label.configure(text="CALEFACTOR: OFF")
                self.heater_circle.configure(text_color="white")
        else:
            temp = base + hyst/2 + random.uniform(-0.5, 0.5)
            if random.random() < 0.1:  # Probabilidad de encendido
                self.heater_state.set(True)
                self.heater_label.configure(text="CALEFACTOR: ON")
                self.heater_circle.configure(text_color="DarkRed")
        
        return temp

    def apply_changes(self):
        """Aplica los cambios de parámetros"""
        self.flag_aplicar_cambios = True
        if self.simulation_mode.get():
            print("Parámetros actualizados en simulación:")
            print(f"setpoint: {self.setpoint.get()}°C")
            print(f"Histéresis: {self.hysteresis.get()}°C")
            print(f"Intervalo: {self.sampling_interval.get()}s")
            self.reset_simulation()
        else:
            self.send_hysteresis(self.hysteresis.get())
            self.send_setpoint(self.setpoint.get())
            self.send_sampling_interval(self.sampling_interval.get())
        self.update_current_values()  # Actualiza las etiquetas

    def send_hysteresis(self, hysteresis_value):
        """Envía el valor de histéresis por el puerto serial en el formato F1XX"""

        # Validar el intervalo antes de aplicarlo
        if not (0.1 <= hysteresis_value <= 10):
            CTkMessagebox.CTkMessagebox(title="Valor Inválido", 
                        message=f"El intervalo debe estar entre {0.1} y {10} ºC.",
                        icon="warning",  # Tipo de mensaje (warning, error, info)
                        option_1="OK")   # Botón de cierre
            return  # Salir sin aplicar cambios

        if not self.simulation_mode.get() and self.ser and self.ser.is_open:
            try:
                command = f"(F2{hysteresis_value:.2f})"
                self.ser.write(command.encode())
                time.sleep(1)
                #print("command")
            except Exception as e:
                print(f"Error enviando histéresis: {str(e)}")

    def send_setpoint(self, setpoint_value):
        """Envía el valor de set point por el puerto serial en el formato F2XX"""
    
        # Validar el intervalo antes de aplicarlo
        if not (20 <= setpoint_value <= 50):
            CTkMessagebox.CTkMessagebox(title="Valor Inválido", 
                        message=f"El intervalo debe estar entre {20} y {50} ºC.",
                        icon="warning",  # Tipo de mensaje (warning, error, info)
                        option_1="OK")   # Botón de cierre
            return  # Salir sin aplicar cambios

        if not self.simulation_mode.get() and self.ser and self.ser.is_open:
            try:
                command = f"(F1{setpoint_value:.2f})"
                self.ser.write(command.encode())
                time.sleep(1)
                #print(f"Enviado: {command}")
            except Exception as e:
                print(f"Error enviando set point: {str(e)}")
    
    def send_sampling_interval(self, interval_value):
        """Envía el valor de set point por el puerto serial en el formato F2XX"""
        
        # Validar el intervalo antes de aplicarlo
        if not (250 <= interval_value <= 32767):
            CTkMessagebox.CTkMessagebox(title="Valor Inválido", 
                        message=f"El intervalo debe estar entre {250} y {32767} ms.",
                        icon="warning",  # Tipo de mensaje (warning, error, info)
                        option_1="OK")   # Botón de cierre
            return  # Salir sin aplicar cambios
        
        if not self.simulation_mode.get() and self.ser and self.ser.is_open:
            try:
                command = f"(F3{interval_value:.2f})"
                self.ser.write(command.encode())
                time.sleep(1)
                #print(f"Enviado: {command}")
            except Exception as e:
                print(f"Error enviando set point: {str(e)}")

    def reset_simulation(self):
        """Reinicia la simulación con nuevos parámetros"""
        self.x_real = []
        self.y_real = []
        self.simulation_start_time = time.time()

    def toggle_simulation(self):
        """Alterna entre modo simulación y real"""
        if self.simulation_mode.get():
            # Modo simulación
            self.update_connection_status("Modo Simulación", "blue")
            if self.ser and self.ser.is_open:
                self.ser.close()
            # Forzar estado OFF en elementos de serial
            self.serial_label.configure(text="CONEXIÓN SERIE: OFF")
            self.serial_circle.configure(text_color="white")
            self.ser = None  # Asegurar que no quede referencia al puerto
        else:
            # Modo real - Intentar reconectar
            self.connect_serial()

    def connect_serial(self):
        """Intenta conectar con el puerto serie solo si no estamos en simulación"""
        if self.simulation_mode.get():  # No conectar si estamos en simulación
            return
            
        try:
            self.ser = serial.Serial(self.serial_port, self.baudrate, timeout=1)
            self.serial_label.configure(text="CONEXIÓN SERIE: ON")
            self.serial_circle.configure(text_color="green")
            self.ser.write('!'.encode())
        except Exception as e:
            print(f"Error al conectar: {str(e)}")
            self.serial_label.configure(text="CONEXIÓN SERIE: OFF")
            self.serial_circle.configure(text_color="white")
            self.ser = None

    def read_serial(self):
        """Lee datos del puerto serie"""
        if self.ser and self.ser.is_open:
            try:
                line = self.ser.readline().decode().strip()
                print(line)
                if line:
                    self.process_serial_data(line)
                    self.ser.write('!'.encode())
                    
            except Exception as e:
                print(f"Error lectura serial: {str(e)}")
                self.ser.close()
                # Actualizar estado a OFF en la GUI
                self.serial_label.configure(text="CONEXIÓN SERIE: OFF")
                self.serial_circle.configure(text_color="white")
                self.ser = None  # Asegurar que el puerto se marque como cerrado

    def process_serial_data(self, data):
        """Procesa datos recibidos del microcontrolador"""
        if data.startswith("(FT"):
            try:
                # Extraer el valor de temperatura del formato (FTXXX)
                temp = float(data[3:-1])
                self.current_temperature.set(f"temperatura: {temp:.1f} °C")
                self.y_real.append(temp)
                self.x_real.append(time.time() - self.simulation_start_time)
                
                if len(self.x_real) > 100:
                    self.x_real.pop(0)
                    self.y_real.pop(0)
            except ValueError:
                print(f"Error al convertir temperatura: {data}")
        elif data.startswith("(F%"):
            try:
                calefactor = float(data[3:-1])
                if calefactor == 1:
                    self.heater_label.configure(text="CALEFACTOR: ON")
                    self.heater_circle.configure(text_color="red")
                else:
                    self.heater_label.configure(text="CALEFACTOR: OFF")
                    self.heater_circle.configure(text_color="white")
            except ValueError:
                print(f"Error al convertir PWM: {data}")
        if data.startswith("(FS"):
            try:
                temp = float(data[3:-1])
                self.current_setpoint.set(f"setpoint: {temp:.1f} °C")
                self.flag_aplicar_cambios = True  # Activar ajuste del gráfico
            except ValueError:
                print(f"Error al convertir el set point: {data}")
        elif data.startswith("(FH"):
            try:
                temp = float(data[3:-1])
                self.current_hysteresis.set(f"hysteresis: {temp:.1f} °C")
                self.flag_aplicar_cambios = True  # Activar ajuste del gráfico
            except ValueError:
                print(f"Error al convertir la histéresis: {data}")
        elif data.startswith("(FP"):
            try:
                temp = float(data[3:-1])
                self.current_interval.set(f"interval: {temp:.1f} ms")
            except ValueError:
                print(f"Error al convertir el intervalo: {data}")

    def send_command(self, command):
        """Envía comandos al microcontrolador"""
        if not self.simulation_mode.get() and self.ser and self.ser.is_open:
            try:
                self.ser.write(f"{command}\n".encode())
            except Exception as e:
                print(f"Error enviando comando: {str(e)}")

    def validate_checksum(self, data):
        """Valida el checksum de los datos"""
        if not data or len(data) < 2:
            return False
        received = data[-1]
        calculated = hashlib.md5((",".join(data[:-1])).encode()).hexdigest()[:8]
        return received == calculated

    def update_connection_status(self, text, color):
        """Actualiza el estado de conexión"""
        self.connection_status.set(text)
        #self.connection_led.configure(text_color=color)

    def reconnect(self):
        """Reintenta la conexión serial"""
        self.connect_serial()

    def on_closing(self):
        """Maneja el cierre de la aplicación"""
        self.reading_data = False
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.destroy()

if __name__ == "__main__":
    app = TemperatureControlGUI()
    app.protocol("WM_DELETE_WINDOW", app.on_closing)
    app.mainloop()