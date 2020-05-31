cbuffer FrameConstants : register(b0){
    matrix porjView;
    matrix view;
    matrix projection;
    matrix invProjection;
    matrix invView;
    matrix invProjView;
    float4 cameraPos;
    float4 lightPosition;
    float4 lightPosInView;
    float4 lightIntensity;
};

cbuffer ObjectConstants : register(b1){
    matrix model;
    float4 ambient;
    float4 diffuse;
    float4 emission;
    float4 specular;
    float specularPower;
};

Texture2D BaseMap : register(t0);
sampler baseSampler : register(s0);

struct VSInput {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 posInView :POSITION;
    float4 normal   : NORMAL;
    float2 uv : TEXCOORD0;
};

PSInput VSMain(VSInput input)
{
    PSInput output;

    matrix MVP = mul(porjView, model);
    output.position = mul(MVP, float4(input.position, 1.0f));
    output.normal = mul(view, mul(model, float4(input.normal, 0.0f)));
    output.uv = input.uv;
    output.posInView = mul(view, mul(model, float4(input.position, 1.0f)));
    return  output;
}

float4 PSMain1(PSInput input) : SV_TARGET
{
	const float4 vN = normalize(input.normal);
	const float4 vL = normalize(lightPosInView - input.posInView);
	const float4 vV = normalize(-input.posInView);
    const float4 vH = normalize(0.5 * (vL + vV));
    const float  r = length(lightPosInView - input.posInView);
    const float invd = 1.0f / (r * r + 1.0f);
	float4 vLightInts = ambient + lightIntensity * invd * invd * (
                              diffuse * max(dot(vN, vL), 0.0f)
                            + specular * pow(max(dot(vH, vN), 0.0f), specularPower)
                        );

	return vLightInts;
}

float4 PSMain2(PSInput input) : SV_TARGET
{

	const float4 vN = normalize(input.normal);
	const float4 vL = normalize(lightPosInView - input.posInView);
	const float4 vV = normalize(-input.posInView);
    const float4 vH = normalize(0.5 * (vL + vV));
    const float  r = length(lightPosInView - input.posInView);
    const float invd = 1.0f / (r * r + 1.0f);
    float gamma = 1;
    float4 vLightInts = ambient + (lightIntensity + 50.0f) * invd * invd * (
							  pow(BaseMap.Sample(baseSampler, input.uv), gamma) * max(dot(vN, vL), 0.0f) 
							+ specular * pow(max(dot(vH, vN), 0.0f), specularPower)
                        );

    vLightInts.rgb = pow(vLightInts.rgb, 1.0f/gamma);
	return vLightInts;
}
