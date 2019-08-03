# dht22_web

code de la lambda
```

import boto3
import json

print('Loading function')
dynamo = boto3.client('dynamodb')


def respond(err, res=None):
    return {
        'statusCode': '400' if err else '200',
        'body': err.message if err else json.dumps(res),
        'headers': {
            'Content-Type': 'application/json',
        },
    }


def lambda_handler(event, context):
    '''Demonstrates a simple HTTP endpoint using API Gateway. You have full
    access to the request and response payload, including headers and
    status code.

    To scan a DynamoDB table, make a GET request with the TableName as a
    query string parameter. To put, update, or delete an item, make a POST,
    PUT, or DELETE request respectively, passing in the payload to the
    DynamoDB API as a JSON body.
    '''
    #print("Received event: " + json.dumps(event, indent=2))

    operations = {
        'DELETE': lambda dynamo, x: dynamo.delete_item(**x),
        'GET': lambda dynamo, x: dynamo.scan(**x),
        'POST': lambda dynamo, x: dynamo.put_item(**x),
#        'POST': lambda dynamo, x: {"message":"post ok"}, # fonctionne
#        'POST': lambda dynamo, x: x,
        'PUT': lambda dynamo, x: dynamo.update_item(**x),
        'ECHO': lambda dynamo, x: {"message":"alu"} # bloqué avant l'appel
    }

    operation = event['httpMethod']
    if operation in operations:
        payload = event['queryStringParameters'] if operation == 'GET' else json.loads(event['body'])
        return respond(None, operations[operation](dynamo, payload))
        #return respond(None, {"bob":"alu"}) # fonctionne
    else:
        return respond(ValueError('Unsupported method "{}"'.format(operation)))
```

dans il constantes.h faut un truc du genre

```
char* ssid = "aze";
const char* password = "aze";
char* logtofileUrl = "https://aze";
char* cleapi = "aze";
const uint8_t fingerprint_aws[20] = {0x44, 0x8B, 0x23, 0xD0, 0x98, 0x49, 0x95, 0xF1, 0x09, 0x27, 0x6D, 0xD2, 0x08, 0xA3, 0x4A, 0xBF, 0x16, 0x06, 0x40, 0xA3};
const uint8_t fingerprint_bin[20] = {0xCD, 0x9A, 0x4D, 0x21, 0xAF, 0x57, 0x65, 0xE6, 0xD3, 0xBE, 0x3A, 0xD0, 0x1C, 0xDC, 0x88, 0x26, 0xB8, 0x96, 0xBB, 0xF7};
```
les fingerprints sont obtenu avec https://www.grc.com/fingerprints.htm

référence httpclient : https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h

référence dynamodb : https://boto3.amazonaws.com/v1/documentation/api/latest/reference/services/dynamodb.html#DynamoDB.Client.put_item
