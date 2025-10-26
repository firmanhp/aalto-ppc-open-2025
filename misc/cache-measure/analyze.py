import json
import matplotlib.pyplot as plt

PLOT_OUT = 'results.png'

with open("results.json") as f:
  data = json.load(f)

test_name = []
test_size_kb = []
test_latencies_ns = []

# Read benchmark data
for bench in data["benchmarks"]:
  if "ns_per_access" not in bench:
    print("Benchmark", bench["name"], "has no counter, ignoring")
    continue
  # BM_CacheSize/1024/manual_time
  parts = bench['name'].split('/')
  if len(parts) < 2 or not parts[1].isdigit():
    continue
  test_name.append(bench['name'])
  test_size_kb.append(int(parts[1]) / 1024)
  test_latencies_ns.append(bench['ns_per_access'])

# Read computer data
cache_lines = [] # (size_kb, name)
for cache_info in data['context']['caches']:
  if cache_info['type'] != 'Data' and cache_info['type'] != 'Unified':
    continue
  size_kb = cache_info['size'] / 1024
  level = cache_info['level']
  typ = cache_info['type']
  cache_lines.append((size_kb, f"L{level} {typ} cache"))
print("Cache lines:", cache_lines)

# Detect big jumps
THRESHOLD = 1.5  # 50% increase considered a cache boundary
boundary_sizes = []
for i in range(len(test_latencies_ns)-1):
    ratio = test_latencies_ns[i+1] / test_latencies_ns[i]
    if ratio > THRESHOLD:
        boundary_sizes.append(test_size_kb[i+1])

TICKS = [2**i for i in range(10, 28)]

plt.figure(figsize=(8,5))
plt.plot(test_size_kb, test_latencies_ns, marker='o')
plt.xscale('log', base=2)
plt.yscale('log', base=2)
plt.xlabel("Working Set Size (KB, log scale)")
plt.ylabel("Latency (ns per access, log scale)")
plt.title("Cache Performance Curve")
plt.grid(True, which="both", linestyle='--', alpha=0.6)

for size_kb, label in cache_lines:
  plt.axvline(x=size_kb, color='r', linestyle='--', alpha=0.7)
  plt.text(size_kb * 1.05, max(test_latencies_ns)*0.9, label, rotation=90,
           color='r', fontsize=9, va='top')

# Draw vertical lines at detected cache boundaries
for size in boundary_sizes:
    plt.axvline(x=size, color='b', linestyle='--', alpha=0.7)
    plt.text(size * 1.05, max(test_latencies_ns)*0.9, f"~{int(size)} KB boundary",
             rotation=90, color='b', fontsize=9, va='top', ha='right')

plt.legend()
plt.tight_layout()
plt.savefig(PLOT_OUT)

print("Plot saved:", PLOT_OUT)
