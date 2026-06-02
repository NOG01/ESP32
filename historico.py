import sqlite3

conn = sqlite3.connect(
    "monitoramento.db"
)

cursor = conn.cursor()

cursor.execute("""
SELECT *
FROM leituras
ORDER BY id DESC
LIMIT 20
""")

dados = cursor.fetchall()

for linha in dados:
    print(linha)
