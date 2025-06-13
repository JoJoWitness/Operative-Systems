cat << EOF >> info.txt
Nombre de la ruta actual: $PWD
Usuario actual: $(whoami)
Cantidad de memoria: $(free -h | grep Mem | awk '{print $2}')
Procesador: $(lscpu | grep -i "Model name:" | cut -d':' -f2- | sed 's/^ *//')
Cantidad de archivos: $(ls | wc -l)
EOF

