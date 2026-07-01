struct VSInput {
  float2 pos;
  float3 col;
  float2 uv;
};

struct VSOutput {
  float4 pos : SV_Position;
  float3 col;
  float2 uv;
};


[shader("vertex")]
VSOutput vertMain(VSInput in) {
  VSOutput out;
  out.pos = float4(1.0 / in.pos.x, 1.0 / in.pos.y, 0.0, 1.0f);
  out.col = in.col;
  out.uv = in.uv;
  return out;
}

[shader("fragment")]
float4 fragMain(VSOutput in) : SV_TARGET {
  //return float4(in.col, 1.0);
  return float4(1.0, 0.0, 0.0, 1.0);
}
