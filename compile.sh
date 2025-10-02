docker start tpe_arqui_2q2025
docker exec -it tpe_arqui_2q2025 make -C /root/Toolchain clean
docker exec -it tpe_arqui_2q2025 make -C /root/Toolchain all
docker exec -it tpe_arqui_2q2025 make -C /root clean
docker exec -it tpe_arqui_2q2025 make -C /root all