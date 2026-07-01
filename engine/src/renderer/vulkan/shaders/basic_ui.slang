struct VSInput {
  float2 pos;
  float3 col;
  float2 uv;
};

ConstantBuffer<float2> aspect;

struct VSOutput {
  float4 pos : SV_Position;
  float3 col;
  float2 uv;
};

[shader("vertex")]
VSOutput vertMain(VSInput in) {
  VSOutput out;
  out.pos = float4((2.0 * (in.pos.x * aspect.x)) - 1.0,
                   (2.0 * (in.pos.y * aspect.y)) - 1.0,
                   0.0, 1.0f);
  out.col = in.col;
  out.uv = in.uv;
  return out;
}

[shader("fragment")]
float4 fragMain(VSOutput in) : SV_TARGET {
  return float4(in.col, 1.0);
}
