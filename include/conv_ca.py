

def generate_header(data, variable_name):
    """Generate c-header from data"""
    chunk_size = 16
    out = []

    #Start of header
    out.append("#ifndef CA_H")
    out.append("#define CA_H")
    
    #Stdint
    out.append("")

    
    #Generate hex code
    out.append('const char {var_name}[] = {{'.format(var_name=variable_name))
    chunks = [ data[i:i+chunk_size] for i in range(0, len(data), chunk_size) ]
    for i, x in enumerate(chunks):
        line = ', '.join([ '0x{val:02x}'.format(val=ord(c)) for c in x ])
        out.append('  {line}{end_comma}'.format(line=line, end_comma=',' if i<len(chunks)-1 else ''))
    out.append('};')
    
    #Add array length
    out.append("")
    out.append('unsigned int {var_name}_len = {data_len};'.format(var_name=variable_name, data_len=len(data)))
    out.append("")
    
    #end of Header
    out.append('#endif')

    return '\n'.join(out)
    

#Generate ca header file.
with open("ca.crt", 'r') as f:
    data = f.read()

#append C 'end of string'
data += '\0' 

out = generate_header(data, "DEFAULT_MQTT_CA")
with open("ca.h", 'w') as f:
    f.write(out)
