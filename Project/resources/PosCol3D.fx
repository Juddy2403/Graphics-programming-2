
//////////////////////////////////
//   Input/Output structs
//////////////////////////////////
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;

float3 gLightDirection : LightDirection = { 0.577f, -0.577f, 0.577f };
float4x4 gWorldMatrix : WORLD;
float3 gCameraPosition : CAMERA;
bool gAreNormalsToggled;

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    //float3 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Color : COLOR;
    float3 ViewDirection : VERTEX_TO_CAMERA;
};

//////////////////////////////////
//   Keeping track of all settings
//////////////////////////////////
DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = ALL;
    DepthFunc = less;
    StencilEnable = false;

    StencilReadMask = 0xFF;
    StencilWriteMask = 0xFF;

    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = always;

    FrontFaceStencilDepthFail = keep;
    BackFaceStencilDepthFail = keep;

    FrontFaceStencilPass = keep;
    BackFaceStencilPass = keep;

    FrontFaceStencilFail = keep;
    BackFaceStencilFail = keep;
};

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

//////////////////////////////////
//   Vertex Shader
//////////////////////////////////
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
   // output.Color = input.Color;
    output.TexCoord = input.TexCoord;
    output.Normal = mul(normalize(input.Normal), (float3x3) gWorldMatrix);
    output.Tangent = mul(normalize(input.Tangent), (float3x3) gWorldMatrix);
    output.ViewDirection = normalize(gCameraPosition - output.Position.xyz);
    return output;
}

//////////////////////////////////
//   Pixel shader
//////////////////////////////////
float3 Phong(float specularReflectance, float phongExp, float3 viewDir, float3 normal)
{
    float3 specularRefl = float3(specularReflectance, specularReflectance, specularReflectance);
    if (phongExp == 0) return specularRefl;
	float3 reflectVar = reflect(gLightDirection,normal);
    const float cos = saturate(dot(reflectVar, viewDir));
	if (phongExp == 1) return specularRefl * cos;
	if (phongExp == 2) return specularRefl * cos * cos;
    return specularRefl * pow(cos, phongExp);
}
float4 PS(VS_OUTPUT input,SamplerState sampleState) : SV_TARGET
{
    float gPI = 3.14;
    float gLightIntensity = 7.0f;
    float gShininess = 25.0f;
    float3 ambient = float3(0.03f, 0.03f, 0.03f);
    
    //Normal mapping
    float3 normal = normalize(input.Normal);
    if (gAreNormalsToggled)
    {
        float3 normalMapSample = gNormalMap.Sample(sampleState, input.TexCoord);
    // Remapping the value from [0,1] to [-1,1] 
        normalMapSample = 2 * normalMapSample - 1;
    // Transforming it in the correct tangent space
        float3 binormal = cross(input.Normal, input.Tangent);
        float3x3 tangentSpaceAxis = float3x3(input.Tangent, binormal, input.Normal);
        normalMapSample = mul(normalMapSample, tangentSpaceAxis);
    //if reading from normal map
        normal = normalMapSample;
    }
    
        float lightDirCos = dot(normal, -gLightDirection);
    
        if (lightDirCos >= 0)
        {
        // Sampling color from maps
            float glossinesMapSample = gGlossinessMap.Sample(sampleState, input.TexCoord);
            float specularMapSample = gSpecularMap.Sample(sampleState, input.TexCoord);
		// SpecularColor sampled from SpecularMap and PhongExponent from GlossinessMap
            float3 specular = Phong(specularMapSample, glossinesMapSample * gShininess, input.ViewDirection, normal);
            float3 cd = gDiffuseMap.Sample(sampleState, input.TexCoord);
            float3 diffuse = cd * gLightIntensity / gPI;
            input.Color = lightDirCos * diffuse + specular + ambient;
            return float4(input.Color, 1.f);
        }
    
        return float4(0.f, 0.f, 0.f, 1.f);
    }
    float4 PS_Point(VS_OUTPUT input) : SV_TARGET
{
    return PS(input, samPoint);
}

float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
    return PS(input, samLinear);
}

float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
    return PS(input, samAnisotropic);
}

//////////////////////////////////
//   Technique
//////////////////////////////////
technique11 DefaultTechnique
{
    pass P0
    {
        SetDepthStencilState(gDepthStencilState, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Point()));
    }

    pass P1
    {
        SetDepthStencilState(gDepthStencilState, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
    }

    pass P2
    {
        SetDepthStencilState(gDepthStencilState, 0);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
    }
}

