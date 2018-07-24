## API methods

All requests to the server should have the /swap prefix
Request format: JSON

**1. Submit a new swap request**
```
{
    "type": "new_order",
    "second_addres": "<address to receive the quote currency of the pair>",
    "first_cur": "<base currency of the pair>",
    "first_count": <amount of base currency for swapping>,
    "second_cur": "<quote currency of the pair>",
    "second_count": <amount of quote currency for swapping>
}
```

**Sample response**
```
{
    "result": "success"
}
```

**2. Respond to swap request and conduct a trade via atomic swap**
```
{
    "type": "new_trade",
    "first_cur": "<base currency of the pair>",
    "first_addres": "<receiver address for base currency of the pair>",
    "second_cur": "<quote currency of the pair>",
    "second_addres": "<receiver address for quote currency of the pair>"
}
```

**Sample response**
```
{
    "result": "success"
}
```

**3. Get a list of swap requests by pair**
```
{
    "type": "orders",
    "first_cur": "<base currency of the pair>",
    "second_cur": "<quote currency of the pair>"
}
```

**Sample response**
```
{
    [
        {
            "order": "orders: << base currency of the pair> _ <quote currency of the pair>: <receiver address for quote currency of the pair>>",
            "first_count": <amount of base currency for swapping>,
            "second_count": <amount of quote currency for swapping>
        }
    ]
}
```

**4. Get a list of all active trades**
```
{
    "type": "trades",
    "first_cur": "<base currency of the pair>",
    "second_cur": "<quote currency of the pair>"
}
```

**Sample response**
```
{
    [
        {
            "key": "trades: << base currency of the pair> _ <quote currency of the pair>: receiver address for quote currency of the pair>>",
            "first_count": <amount of base currency for swapping>,
            "second_count": <amount of quote currency for swapping>
        }
    ]
}
```

**5. Get a trade by the address**
```
{
    "type": "trade",
    "second_addres": "<receiver address for quote currency of the pair>",
    "first_cur": "<base currency of the pair>",
    "second_cur": "<quote currency of the pair>",
}
```

**Sample response**
```
{
    "key": "trades: << base currency of the pair> _ <quote currency of the pair>: <receiver address for quote currency of the pair>>",
    "first_count": <amount of base currency for swapping>,
    "second_count": <amount of quote currency for swapping>
}
```

In case a trade cannot be found: 
```
{
    result": "trade not found"
}
```

**6. Initiate swap**
```
{
    "type": "swap",
    "first_cur": "<base currency of the pair>",
    "first_addres": "<receiver address for base currency of the pair>",
    "second_cur": "<quote currency of the pair>",
    "second_addres": "<receiver address for quote currency of the pair>",
    "secret_hash": "<private key hash>",
either
    "first_contract": "<hash of a swap contract for base currency of the pair>",
    "first_contract_tx": "<hash of a contract transaction in the blockchain of base currency of the pair>"
or
    "second_contract": "<hash of a swap contract for quote currency of the pair>",
    "second_contract_tx": "<hash of a contract transaction in the blockchain of quote currency of the pair>"
}
```

* this request must contain information about the contract either for base or for quote currency of the pair, otherwise the server will return an error "invalid JSON"

**Sample response**
```
{
    "result": "success"
}
```

**7. Add a hash of redemption transaction to the swap**
```
{
    "type": "redeem_swap",
    "first_cur": "<base currency of the pair>",
    "first_addres": "<receiver address for base currency of the pair>",
    "second_cur": "<quote currency of the pair>",
    "second_addres": "<receiver address for quote currency of the pair>",
either
    "first_redemption_tx": "<hash of the contract redemption transaction in the blockchain of base currency of the pair>"
or
    "second_redemption_tx": "<hash of the contract redemption transaction in the blockchain of quote currency of the pair>"
}
```

* this request must contain information about the redemption transaction either for base or for quote currency of the pair, otherwise the server will return an error "invalid JSON"

**Sample response**
```
{
    "result": "success"
}
```

**8. Get swap info**
```
{
    "type": "get_swap",
    "first_cur": "<base currency of the pair>",
    "first_addres": "<receiver address for base currency of the pair>",
    "second_cur": "<quote currency of the pair>",
    "second_addres": "<receiver address for quote currency of the pair>"
}
```

**Sample response**
```
{
    "secret_hash": "<private key hash>",
either
    "first":
    {
        "first_contract": "<hash of a swap contract for base currency of the pair>"
        "first_contract_tx": "<hash of a contract transaction in the blockchain of base currency of the pair>"

        additionally (once the redemption transaction has been saved)

        "first_redemption_tx": "<hash of the contract redemption transaction in the blockchain of base currency of the pair>"
    },
or
    "second":
    {
        "second_contract": "<hash of a swap contract for quote currency of the pair>"
        "second_contract_tx": "<hash of a contract transaction in the blockchain of quote currency of the pair>"

        additionally (once the redemption transaction has been saved)

        "second_redemption_tx": "<hash of the contract redemption transaction in the blockchain of quote currency of the pair>"
    },
}
```

* this response always contains either 'first' or 'second' or both JSON objects

In case a swap cannot be found: 
```
{
    "result": "swap not found"
}
```

**9. Finalize swap**
```
{
    "type": "swap",
    "first_cur": "<base currency of the pair>",
    "first_addres": "<receiver address for base currency of the pair>",
    "second_cur": "<quote currency of the pair>",
    "second_addres": "<receiver address for quote currency of the pair>"
either
    "first_close": <boolean flag to finalize swap for base currency>
or
    "second_close": <boolean flag to finalize swap for quote currency>
}
```

* this request correctly handles the case of both boolean flags set to 'true', in this case the swap is completely finalized

**Sample response**
```
{
    "result": "success"
}
```

