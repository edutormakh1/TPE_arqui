#!/bin/bash
set -e

IMG="Image/x64BareBonesImage.qcow2"
MEM=512

# Detectar backend disponible
DRIVERS=$(qemu-system-x86_64 -audiodev help 2>/dev/null || true)
if echo "$DRIVERS" | grep -qw pa; then
  BACKEND=pa
elif echo "$DRIVERS" | grep -qw alsa; then
  BACKEND=alsa
else
  BACKEND=sdl   # fallback
fi

CMD_NEW="qemu-system-x86_64 -m $MEM -drive file=$IMG,if=ide,format=qcow2 \
  -audiodev ${BACKEND},id=snd0 -machine pcspk-audiodev=snd0"

echo "$CMD_NEW"
if $CMD_NEW 2>/dev/null; then
  exit 0
fi

echo "Fallo -audiodev. Probando modo legacy..."
# Legacy: SIN -audio (no existe). Solo variable de entorno + -soundhw pcspk
exec env QEMU_AUDIO_DRV=${BACKEND} qemu-system-x86_64 -m $MEM \
  -drive file=$IMG,if=ide,format=qcow2 -soundhw pcspk