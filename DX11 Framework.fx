//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Texture and sampler register setting
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Lighting structures, They are mirrorerd in structures.h
//--------------------------------------------------------------------------------------
struct SurfaceInfo
{
    float4 AmbientMtrl;
    float4 DiffuseMtrl;
    float4 SpecularMtrl;
};


struct DirectionalLight
{
    float4 AmbientLight;
    float4 DiffuseLight;
    float4 SpecularLight;
    
    float SpecularPower;
    float3 Direction;
};

struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

	//Packed into 4D vector: (Position, Range)
    float3 Position;
    float Range;
	//Packed into 4D Vector: (A0, A1, A2, pad)
    float3 Att;
    float pad; // Pad the last float so we can set an
	// array of lights if we wanted.
};

struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

	// Packed into 4D vector: (Position, Range)
    float3 Position;
    float Range;
	// Packed into 4D vector: (Direction,Spot)
    float3 Direction;
    float Spot;
	// Packed into 4D vector: (Att, Pad)
    float3 Att;
    float Pad; // Pad the last float so we can set an
	// array of lights if we wanted.
    
};
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
	matrix World;	//4
	matrix View;		//4
	matrix Projection;		//4
    
    SurfaceInfo surface;
    DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
    
	float3 EyePosW;		//3
    float HasTexture; //1
}

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSTION;
    
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD;
};


//--------------------------------------------------------------------------------------
// Vertex Shader output
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT vin)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;

    float4 posW = mul(vin.Pos, World);
    output.PosW = posW.xyz;

    output.PosH = mul(posW, View);
    output.PosH = mul(output.PosH, Projection);
    output.Tex = vin.Tex;

    float3 normalW = mul(float4(vin.Norm, 0.0f), World).xyz;
    output.Norm = normalize(normalW);

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT pin) : SV_Target
{
    float3 normalW = normalize(pin.Norm);

    float3 toEye = normalize(EyePosW - pin.PosW);

	// Get texture data from file
    float4 textureColour = txDiffuse.Sample(samLinear, pin.Tex);

    float3 ambient = float3(0.0f, 0.0f, 0.0f);
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    float3 specular = float3(0.0f, 0.0f, 0.0f);

    float3 lightLecNorm = normalize(gDirLight.Direction);
	// Compute Colour

	// Compute the reflection vector.
    float3 r = reflect(-lightLecNorm, normalW);

	// Determine how much specular light makes it into the eye.
    float specularAmount = pow(max(dot(r, toEye), 0.0f), gDirLight.SpecularPower);

	// Determine the diffuse light intensity that strikes the vertex.
    float diffuseAmount = max(dot(lightLecNorm, normalW), 0.0f);

	// Only display specular when there is diffuse
    if (diffuseAmount <= 0.0f)
    {
        specularAmount = 0.0f;
    }

	// Compute the ambient, diffuse, and specular terms separately.
    specular += specularAmount * (surface.SpecularMtrl * gDirLight.SpecularLight).rgb;
    diffuse += diffuseAmount * (surface.DiffuseMtrl * gDirLight.DiffuseLight).rgb;
    ambient += (surface.AmbientMtrl * gDirLight.AmbientLight).rgb;

	// Sum all the terms together and copy over the diffuse alpha.
    float4 finalColour;

    if (HasTexture == 1.0f)
    {
        finalColour.rgb = (textureColour.rgb * (ambient + diffuse)) + specular;
        //finalColour.rgb = specular;
    }
    else
    {
        finalColour.rgb = ambient + diffuse + specular;
    }

    finalColour.a = surface.DiffuseMtrl.a;

    return finalColour;
}


///////////////////////////////////
// Old code Snippets for referal //
///////////////////////////////////

//--------------------------------------------------------------------------------------
// Vertex Shader - Implements Gouraud Shading using Diffuse lighting only
//--------------------------------------------------------------------------------------
//VS_OUTPUT VS(float4 Pos : POSITION, float3 NormalL : NORMAL)
//{
//    VS_OUTPUT output = (VS_OUTPUT)0;
//
//    output.Pos = mul(Pos, World);
//    output.Pos = mul(output.Pos, View);
//    output.Pos = mul(output.Pos, Projection);
//
//    // Convert from local space to world space 
//    // W component of vector is 0 as vectors cannot be translated
//    float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
//    normalW = normalize(normalW);
//
//    // Compute Colour using Diffuse lighting only
//    float diffuseAmount = max(dot(LightVecW, normalW), 0.0f);
//    output.Col.rgb = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;
//    output.Col.a = DiffuseMtrl.a;
//
//
//    //Pos.xy += 0.5f * sin(Pos.x) * sin(3.0f * gTime);
//    //Pos.z *= 0.6f + 0.4f * sin(2.0f * gTime);
//    return output;
//}


//--------------------------------------------------------------------------------------
// Vertex Shader - Uses Ambient lighting
//--------------------------------------------------------------------------------------
//VS_OUTPUT VS(float4 Pos : POSITION, float3 NormalL : NORMAL)
//{
//    VS_OUTPUT output = (VS_OUTPUT)0;
//
//    output.Pos = mul( Pos, World );
//    output.Pos = mul( output.Pos, View );
//    output.Pos = mul( output.Pos, Projection );
//    
//    // Convert from local space to world space 
//    // W component of vector is 0 as vectors cannot be translated
//    float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
//    normalW = normalize(normalW);
//
//    //Compute Color using ambient and Diffuse lighting
//    //Calculate diffuse amount
//    float diffuseAmount = max(dot(LightVecW, normalW), 0.0f);  
//    //Calculate diffuse component
//    float3 diffuse = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;  
//    //Calculate ambient component
//    float3 ambient = AmbientMtrl * AmbientLight;    
//    output.Col.rgb = diffuse + ambient;
//    output.Col.a = DiffuseMtrl.a;
//
//    return output;
//}


//--------------------------------------------------------------------------------------
// Vertex Shader - Uses Specular lighting
//--------------------------------------------------------------------------------------
//VS_OUTPUT VS(float4 Pos : POSITION, float3 NormalL : NORMAL)
//{
//    VS_OUTPUT output = (VS_OUTPUT)0;
//    output.Pos = mul(Pos, World);
//
//    //Compute the vector from the vertex to the eye position
//    // output.Pos is currently the position in world space
//    float3 toEye = normalize(EyePosW - output.Pos.xyz);
//
//    // Apply View and Projection transformations
//    output.Pos = mul(output.Pos, View);
//    output.Pos = mul(output.Pos, Projection);
//
//    // Convert from local space to world space 
//    // W component of vector is 0 as vectors cannot be translated
//    float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
//    normalW = normalize(normalW);
//
//    //Compute colour
//    //Compute the reflection vector.
//    float3 r = reflect(-LightVecW, normalW);
//
//    //Determine how much (if any) specular light makes it in to the eye
//    float specularAmount = pow(max(dot(r, toEye), 0.0f), SpecularPower);
//
//    //Calculate diffuse and ambient lighting
//    float diffuseAmount = max(dot(LightVecW, normalW), 0.0f);
//    float3 diffuse = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;
//    float3 ambient = AmbientMtrl * AmbientLight;
//
//    //Compute the ambient, diffuse and specular terms seperately
//    float3 specular = specularAmount * (SpecularMtrl * SpecularLight).rgb;
//
//    //Sum all the terms together anf copy over the diffuse alpha
//    output.Col.rgb = diffuse + ambient + specular;
//    output.Col.a = DiffuseMtrl.a;
//   
//    return output;
//}