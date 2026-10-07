# RAVE Profiler - Extended version
## 1. Configuración del Entorno

Todas las variables de entorno(configuración de RAVE) están definidas en el archivo `entorno.sh`. Puedes abrir y editar este archivo libremente para modificar parámetros de la simulación. 

Para aplicar estas variables sobre tu sesión de terminal actual, ejecuta siempre este comando antes de empezar a trabajar:

```bash
source entorno.sh

```

Dentro del script, puedes modificar manualmente la ventana de distancia (en número de instrucciones) que el *profiler* utiliza para detectar y contabilizar los riesgos estructurales y de datos. Por ejemplo:

```bash
export RAVE_RAW_DIST=8
export RAVE_WAR_DIST=20
export RAVE_WAW_DIST=8

```

## 2. Modos de Ejecución y Traza

El nivel de profundidad del análisis del *profiler* se controla mediante la variable de entorno `TRACE_EXTENDED`. Debes configurar esta variable antes de compilar/ejecutar:

### 2.1 Modo Estándar

Realiza un perfilado básico de instrucciones, ancho de banda y métricas generales. Es el por defecto de RAVE.

```bash
export TRACE_EXTENDED=0

```

### 2.2 Modo de Traza Extendida

Activa la categorización avanzada de instrucciones de 16 bits (FMA/Fused, Widening, Narrowing, Moves) y el motor de rastreo de riesgos de datos (RAW, WAW, WAR).

```bash
export TRACE_EXTENDED=1

```
- **¿Qué incluye la traza extendida (por el momento)?**
	- Avg VL, LMUL y ocupación de registros (En % de bits respecto a VLMAX).
	- Número de registros accedidos.
	- Cuantas veces tu/ta afecta (**TamVEctorReal < VLEN**)
	- Cantidad de dependencias vectoriales con distancia máxima configurable para cada tipo de dependencia.
	- Mix de instrucciones redistribuido y ampliado de la siguiente manera:
		- **Arith:** Separado a su vez en FP/INT y con una métrica de cuantas instrucciones escriben en registros escalares y leen de registros escalares. 
			- **Widening:** Reduction, Fused.
			- **Narrowing**
			- **Others:** Computation, reduction, mask (instrucciones que modifican los vectores de mascaras), permutations y moves (separados en si leen de registros escalares, si escriben en registros escalares o si mueve datos unicamente entre registros vectoriales)
		- **Memory:** Se mantiene prácticamente igual pero se añade si son ordered/unordered, para L/S indexed; y si son L/S segmentados para unit, strided e indexed. También se añade la desviación típica del stride para las instrucciones strided.

## 3. Instrucciones no compatibles con RAVE/QEMU
`vfwredusum.vs`
`vlseg8e32.v`
