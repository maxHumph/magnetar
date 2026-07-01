struct VSInput {
  float2 pos;
  float3 col;
  float2 uv;
};

ConstantBuffer<float4x4> ui_mat;

struct VSOutput {
  float4 pos : SV_Position;
  float3 col;
  float2 uv;
};

[shader("vertex")]
VSOutput vertMain(VSInput in) {
  VSOutput out;
  out.pos = mul(ui_mat, float4(in.pos.x, in.pos.y, 0.0, 1.0));

  out.col = in.col;
  out.uv = in.uv;
  return out;
}

[shader("fragment")]
float4 fragMain(VSOutput in) : SV_TARGET {
  return float4(in.col, 1.0);
}
