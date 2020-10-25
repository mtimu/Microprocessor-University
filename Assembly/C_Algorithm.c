

void insertionSort(int arr[], int n) 
{ 

    int right, rightVal, left; 

    for (right = 1; right < n; right++) { 

        rightVal = arr[right]; 

        left = right - 1; 
  

        /* Move elements of arr[0..i-1], that are 
          greater than key, to one position ahead 
          of their current position */

        while (left >= 0 && arr[left] > rightVal) { 

            arr[left + 1] = arr[left]; 

            left--;

        } 

        arr[left + 1] = rightVal; 

    } 

} 
  