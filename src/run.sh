#!/usr/bin/env bash
#
# Uso: ./run.sh <modo> [--prod N] [--cons M] [--buffer S] [--runtime T]
# mod os: nosync | sem | mutex | monitor

if [ $# -lt 1 ]; then
  echo "Uso: $0 <nosync|sem|mutex|monitor> [--prod N] [--cons M] [--buffer S] [--runtime T]"
  exit 1
fi

mode=$1; shift

case "$mode" in
  nosync)  exe=prodcons_nosync ;;
  sem)     exe=prodcons_sem    ;;
  mutex)   exe=prodcons_mutex  ;;
  monitor) exe=prodcons_monitor;;
  *)
    echo "Modo inválido: $mode"
    exit 1
    ;;
esac

# Passa todos os outros parâmetros para o executável escolhido
./$exe "$@"
