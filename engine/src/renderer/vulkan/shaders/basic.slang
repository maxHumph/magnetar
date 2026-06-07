struct VSInput {
    float2 in_position;
    float3 in_colour;
};  

struct VSOutput {
    float4 position : SV_Position;
    float3 colour;
};

[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;
    output.position = float4(input.in_position, 0.0, 1.0);
    output.colour = input.in_colour;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vert_in) : SV_TARGET {
    return float4(vert_in.colour, 1.0);
}
