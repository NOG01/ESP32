import sqlite3
import json
from datetime import datetime

import paho.mqtt.client as mqtt

DB = "monitoramento.db"

conn = sqlite3.connect(DB)
cursor = conn.cursor()

cursor.execute("""
CREATE TABLE IF NOT EXISTS leituras(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    data_hora TEXT,
    temperatura REAL,
    vibracao INTEGER
)
""")

conn.commit()


def salvar(temp, vib):

    cursor.execute("""
    INSERT INTO leituras(
        data_hora,
        temperatura,
        vibracao
    )
    VALUES(?,?,?)
    """,
    (
        datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        temp,
        vib
    ))

    conn.commit()


def on_connect(client, userdata, flags, rc):

    print("Conectado ao MQTT")

    client.subscribe("industria/sensores")


def on_message(client, userdata, msg):

    dados = json.loads(msg.payload.decode())

    temperatura = dados["temperatura"]
    vibracao = dados["vibracao"]

    salvar(
        temperatura,
        vibracao
    )

    print("=" * 50)
    print("NOVA LEITURA")
    print("Temperatura:", temperatura)
    print("Vibração:", vibracao)

    if temperatura > 35:
        print("ALERTA: TEMPERATURA ALTA")

    if vibracao == 1:
        print("ALERTA: VIBRAÇÃO DETECTADA")


client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message

client.connect(
    "localhost",
    1883,
    60
)

client.loop_forever()
