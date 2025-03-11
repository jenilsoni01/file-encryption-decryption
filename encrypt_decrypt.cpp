#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <algorithm> // For remove_if

using namespace std;

// Convert string to hex representation (readable format)
string toHex(const string &input) {
    stringstream ss;
    for (unsigned char c : input) {
        ss << hex << setw(2) << setfill('0') << (int)c;
    }
    return ss.str();
}

// Convert hex back to original string
string fromHex(const string &input) {
    string output;
    for (size_t i = 0; i < input.length(); i += 2) {
        int value;
        stringstream ss;
        ss << hex << input.substr(i, 2);
        ss >> value;
        output += static_cast<char>(value);
    }
    return output;
}

// XOR Encrypt/Decrypt
void encryptDecrypt(const string &fileName, const string &key, bool isEncrypting) {
    fstream file(fileName, ios::in | ios::out);
    
    if (!file) {
        cerr << "Error opening file: " << fileName << endl;
        return;
    }

    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
    
    if (content.empty()) {
        cerr << "Error: File is empty!" << endl;
        return;
    }

    size_t keyLength = key.length();
    string processedData;

    if (isEncrypting) {
        // XOR encryption
        for (size_t i = 0; i < content.size(); ++i) {
            processedData += content[i] ^ key[i % keyLength];
        }

        // Convert to readable format (hex)
        string hexEncrypted = toHex(processedData);
        string hexKey = toHex(key);  // Convert key to hex

        // Store encrypted content + key at the end
        ofstream outFile(fileName, ios::trunc);
        outFile << hexEncrypted << "\nKEY:" << hexKey << endl;
        outFile.close();

    } else {
        // Extract encrypted data and key
        size_t keyPos = content.rfind("\nKEY:");  // Find the last occurrence of "KEY:"
        if (keyPos == string::npos) {
            cerr << "Corrupt file: Key missing!" << endl;
            return;
        }

        string hexEncrypted = content.substr(0, keyPos);
        string hexKey = content.substr(keyPos + 5);  // Remove "KEY:"

        // Trim unwanted characters from hexKey
        hexKey.erase(remove_if(hexKey.begin(), hexKey.end(), ::isspace), hexKey.end());

        string encryptedData = fromHex(hexEncrypted);
        string extractedKey = fromHex(hexKey);

        if (extractedKey.empty()) {
            cerr << "Error: Extracted key is empty!" << endl;
            return;
        }

        // XOR decryption
        string decryptedData;
        for (size_t i = 0; i < encryptedData.size(); ++i) {
            decryptedData += encryptedData[i] ^ extractedKey[i % extractedKey.length()];
        }

        // Write back decrypted content
        ofstream outFile(fileName, ios::trunc);
        outFile << decryptedData;
        outFile.close();
    }

    cout << "Process completed: " << fileName << endl;
}

int main() {
    string fileName, key;
    int choice;

    cout << "File Encryption & Decryption Program\n";
    cout << "1. Encrypt a file\n";
    cout << "2. Decrypt a file\n";
    cout << "Enter your choice (1/2): ";
    cin >> choice;

    cout << "Enter file name: ";
    cin >> fileName;

    if (choice == 1) {
        cout << "Enter encryption key: ";
        cin >> key;
        encryptDecrypt(fileName, key, true);
    } else if (choice == 2) {
        encryptDecrypt(fileName, "", false);  // Extract key from file
    } else {
        cout << "Invalid choice!" << endl;
    }

    return 0;
}
