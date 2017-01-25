float distance = length(LIGHT_POSITION_WORLDSPACE_2 - Vertex_Position_In_Worldspace);

l = normalize(LightDirection2);

vec3 E = normalize(EyeDirection);
vec3 R = reflect(-l,n);
float cosAlpha = clamp( dot( E,R ), 0,1 );

float cosTheta = clamp(dot(n,l), 0, 1);

// Output color = color of the texture at the specified UV
color += MaterialDiffuseColor * LightColor2 * LightPower2 * cosTheta / (distance*distance) +
MaterialSpecularColor * LightColor2 * LightPower2 * pow(cosAlpha,5) / (distance*distance);
