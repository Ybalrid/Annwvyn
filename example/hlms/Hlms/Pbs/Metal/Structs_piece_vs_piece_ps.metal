@piece( PassStructDecl )
struct ShadowReceiverData
{
	float4x4 texViewProj;
	float2 shadowDepthRange;
	float2 padding;
	float4 invShadowMapSize;
};

struct Light
{
	float3 position;
	float3 diffuse;
	float3 specular;
@property( hlms_num_shadow_maps )
	float3 attenuation;
	float3 spotDirection;
	float3 spotParams;
@end
};

@insertpiece( DeclCubemapProbeStruct )

//Uniforms that change per pass
struct PassData
{
	//Vertex shader (common to both receiver and casters)
	float4x4 viewProj;

@property( !hlms_shadowcaster )
	//Vertex shader
	float4x4 view;
	@property( hlms_num_shadow_maps )ShadowReceiverData shadowRcv[@value(hlms_num_shadow_maps)];@end

	//-------------------------------------------------------------------------

	//Pixel shader
	float3x3 invViewMatCubemap;

@property( hlms_use_prepass )
	float4 windowHeight;
@end

@property( ambient_hemisphere || ambient_fixed || envmap_scale )
	float4 ambientUpperHemi;
@end
@property( ambient_hemisphere )
	float4 ambientLowerHemi;
	float4 ambientHemisphereDir;
@end
@property( irradiance_volumes )
	float4 irradianceOrigin;	//.w = maxPower
	float4 irradianceSize;		//.w = 1.0f / irradianceTexture->getHeight()
	float4x4 invView;
@end
@property( hlms_pssm_splits )@foreach( hlms_pssm_splits, n )
	float pssmSplitPoints@n;@end @end
	@property( hlms_lights_spot )Light lights[@value(hlms_lights_spot)];@end
@end @property( hlms_shadowcaster )
	//Vertex shader
	float2 depthRange;
@end

@property( hlms_forwardplus )
	//Forward3D
	//f3dData.x = minDistance;
	//f3dData.y = invMaxDistance;
	//f3dData.z = f3dNumSlicesSub1;
	//f3dData.w = uint cellsPerTableOnGrid0 (floatBitsToUint);

	//Clustered Forward:
	//f3dData.x = minDistance;
	//f3dData.y = invExponentK;
	//f3dData.z = f3dNumSlicesSub1;
	//f3dData.w = renderWindow->getHeight();
	float4 f3dData;
	@property( hlms_forwardplus == forward3d )
		float4 f3dGridHWW[@value( forward3d_num_slices )];
	@end
	@property( hlms_forwardplus != forward3d )
		float4 fwdScreenToGrid;
	@end
@end

@property( parallax_correct_cubemaps )
	CubemapProbe autoProbe;
@end

	@insertpiece( custom_passBuffer )
};@end

@piece( PassDecl )
, constant PassData &pass [[buffer(CONST_SLOT_START+0)]]
@end

@property( fresnel_scalar )@piece( FresnelType )float3@end @piece( FresnelSwizzle )xyz@end @end
@property( !fresnel_scalar )@piece( FresnelType )float@end @piece( FresnelSwizzle )x@end @end

@piece( MaterialStructDecl )
//Uniforms that change per Item/Entity, but change very infrequently
struct Material
{
	/* kD is already divided by PI to make it energy conserving.
	  (formula is finalDiffuse = NdotL * surfaceDiffuse / PI)
	*/
	float4 bgDiffuse;
	float4 kD; //kD.w is alpha_test_threshold
	float4 kS; //kS.w is roughness
	//Fresnel coefficient, may be per colour component (float3) or scalar (float)
	//F0.w is transparency
	float4 F0;
	float4 normalWeights;
	float4 cDetailWeights;
	float4 detailOffsetScaleD[4];
	float4 detailOffsetScaleN[4];

	//uint4 indices0_3;
	ushort diffuseIdx;
	ushort normalIdx;
	ushort specularIdx;
	ushort roughnessIdx;
	ushort weightMapIdx;
	ushort detailMapIdx0;
	ushort detailMapIdx1;
	ushort detailMapIdx2;

	//uint4 indices4_7;
	ushort detailMapIdx3;
	ushort detailNormMapIdx0;
	ushort detailNormMapIdx1;
	ushort detailNormMapIdx2;
	ushort detailNormMapIdx3;
	ushort envMapIdx;
	float mNormalMapWeight;
};@end

@piece( MaterialDecl )
, constant Material *materialArray [[buffer(CONST_SLOT_START+1)]]
@end


@piece( InstanceDecl )
//Uniforms that change per Item/Entity
//.x =
//The lower 9 bits contain the material's start index.
//The higher 23 bits contain the world matrix start index.
//
//.y =
//shadowConstantBias. Send the bias directly to avoid an
//unnecessary indirection during the shadow mapping pass.
//Must be loaded with uintBitsToFloat
, constant uint4 *worldMaterialIdx [[buffer(CONST_SLOT_START+2)]]
@end

@property( envprobe_map && envprobe_map != target_envprobe_map && use_parallax_correct_cubemaps )
@piece( PccManualProbeDecl )
, constant CubemapProbe &manualProbe [[buffer(CONST_SLOT_START+3)]]
@end
@end

//Reset texcoord to 0 for every shader stage (since values are preserved).
@pset( texcoord, 0 )

@piece( VStoPS_block )
	@property( !hlms_shadowcaster )
		@property( !lower_gpu_overhead )
			ushort materialId [[flat]];
		@end
		@property( hlms_normal || hlms_qtangent )
			float3 pos;
			float3 normal;
			@property( normal_map )float3 tangent;
				@property( hlms_qtangent )float biNormalReflection [[flat]];@end
			@end
		@end
		@foreach( hlms_uv_count, n )
			float@value( hlms_uv_count@n ) uv@n;@end

		@foreach( hlms_num_shadow_maps, n )
			float4 posL@n;@end

		@property( hlms_pssm_splits )float depth;@end
	@end

	@property( hlms_shadowcaster )
		@property( alpha_test )
			ushort drawId [[flat]];
			@foreach( hlms_uv_count, n )
				float@value( hlms_uv_count@n ) uv@n;@end
		@end
		@property( !hlms_shadow_uses_depth_texture )
			float depth;
		@end
	@end

	@insertpiece( custom_VStoPS )
@end
