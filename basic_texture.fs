#version 460

uniform sampler2D texture;
uniform float alpha;

in vec2 texCoord;
in vec3 pixelNormal;
in vec3 crntPos;

layout (location=0) out vec4 fragColour;

vec3 calcLightContributionForPointLight(vec3 vertexPos, vec3 vertexNormal, vec3 L, vec3 Lcolour, vec3 Lk) {

	//Distance from vertex to light position
	vec3 D = L - vertexPos;
	//get length of distance vector
	float lengthD = length(D);
	//normalize distance vector
	vec3 D_ = normalize(D);

	//calculate attenuation
	float a = 1.0 / (Lk.x + Lk.y * lengthD + Lk.z * lengthD * lengthD);

	//calculate lambertian
	float lam = clamp(dot(D_, vertexNormal), 0, 1);

	return lam * a * Lcolour;
}

void main(void) {

	//setup light locations
	vec3 L[2];
	L[0] = vec3(2.0, 2.0, 1.0);
	L[1] = vec3(-2.0, 2.0, 1.0);

	//attenuation properties
	vec3 Lk = vec3(1.0, 0.5, 0.0); 

	//light colour
	vec3 Lcolour = vec3(0.6, 0.6, 0.6);

	//ambient light
	float ambientIntensity = .2f;
	vec3 ambientColour = Lcolour * ambientIntensity;

	//setup brightness and diffuse
	vec3 diffuseColour = vec3(1.f, 1.f, 1.f);
	vec3 lightDirection = vec3(0.0, 0.0, 0.0);
	vec3 brightness = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < 2; ++i) {
		brightness += calcLightContributionForPointLight(crntPos.xyz, pixelNormal, L[i], Lcolour, Lk);
		lightDirection = normalize(L[i] - crntPos);
	}
	float diffuseTemp = max(dot(pixelNormal, lightDirection), 0.0f);
	vec3 diffuse = diffuseTemp * diffuseColour;

	//add lighting onto pixel
	vec4 textureLigting = vec4(ambientColour, 1.0) + vec4(diffuse, 1.0) + vec4(brightness, 1.0);
	//add lighting and texture onto final pixel colour
	fragColour = texture2D(texture, texCoord) * textureLigting;
	//fragColour = vec4(pixelNormal, 1.0);

	//add completed lighting to final colour output, adding in transparency
	//fragColour = vec4(textureLigting.xyz, alpha);
    
}
