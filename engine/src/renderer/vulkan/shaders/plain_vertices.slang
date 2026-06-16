struct VSInput {
    float3 position;
    float3 colour;
    float2 tex_coord;
};  

struct MVPMat {
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};
ConstantBuffer<MVPMat> mvp_mat;

struct VSOutput {
    float4 position : SV_Position;
    float3 colour;
    float2 tex_coord;
};

[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;
    output.position = mul(mvp_mat.proj, mul(mvp_mat.view, mul(mvp_mat.model, float4(input.position, 1.0))));
    output.colour = input.colour;
    output.tex_coord = input.tex_coord;
    return output;
}


[shader("fragment")]
float4 fragMain(VSOutput vert_in) : SV_TARGET {
  return float4(0.5, 0.5, 0.5, 1.0);
}
